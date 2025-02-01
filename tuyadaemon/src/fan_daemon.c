#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#include "cJSON.h"
#include "tuya_error_code.h"
#include "system_interface.h"
#include "mqtt_client_interface.h"
#include "tuyalink_core.h"
#include "message_logic.h"
#include "tuya_logic.h"

//Vars

//Signals

void handle_signal(int signal) {
    switch (signal) {
        case SIGTERM:

        case SIGINT:
            closelog();
            tuya_mqtt_disconnect(&client_instance);
            tuya_mqtt_deinit(&client_instance);
            ubus_free(ctx);
            exit(0);
            break;

        case SIGHUP:
            flag = 1;
            break;

        default:
            break;
    }
}

//ARGP

const char *argp_program_version = "SysInfo Tuya cloud interface 1.1";
const char *argp_program_bug_address ="<matas.zilinskas@teltonika.lt>";
 
/* Program documentation. */
static char doc[] =
  "Daemon used to represent a smart fan for tuya cloud";
 
/* A description of the arguments we accept. */
/* A description of the arguments we accept. */
static char args_doc[] = "ProductID, DeviceID, DeviceSecret, foreground mode";
 
static struct argp_option options[] = {
    {"product-id", 'p', "PRODUCTID", 0, "Product ID"},
    {"device-id", 'd', "DEVICEID", 0, "Device ID"},
    {"device-secret", 's', "DEVICESECRET", 0, "Device Secret"},
    {"foreground", 'f', 0, 0, "Run in foreground mode"},
    {0}
};
 
/* Parse a single option */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch (key) {
        case 'p':
            strncpy(arguments->productId, arg, sizeof(arguments->productId) - 1);
            arguments->productId[sizeof(arguments->productId) - 1] = '\0';
            break;
        case 'd':
            strncpy(arguments->deviceId, arg, sizeof(arguments->deviceId) - 1);
            arguments->deviceId[sizeof(arguments->deviceId) - 1] = '\0';
            break;
        case 's':
            strncpy(arguments->deviceSecret, arg, sizeof(arguments->deviceSecret) - 1);
            arguments->deviceSecret[sizeof(arguments->deviceSecret) - 1] = '\0';
            break;
        case 'f':
            arguments->foreground = true;
            break;
        case ARGP_KEY_ARG:
            // We do not expect any positional arguments since all are flags.
            return ARGP_ERR_UNKNOWN;
        case ARGP_KEY_END:
            // Ensure all required options are provided
            if (!arguments->productId[0] || !arguments->deviceId[0] || !arguments->deviceSecret[0]) {
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}
 
/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

//Daemon

static void skeleton_daemon()
{
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }
}

int main(int argc, char** argv)
{
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);
    signal(SIGHUP, handle_signal);

    openlog("Tuya cloud interface daemon", LOG_PID | LOG_CONS, LOG_USER);

    struct arguments arguments;

    arguments.foreground = false; // Default value for the foreground flag
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    if(!arguments.foreground) {
        skeleton_daemon();
    }
 
    tuya_mqtt_context_t* client = &client_instance;

    int ret = init_ubus();
    
    if( ret==0 ){
        ret = tuya_main(&arguments, client);
        tuya_loop(client);
    }

    closelog();
    if(tuya_mqtt_connected){
        tuya_mqtt_disconnect(client);
    }
    if(mqtt_initialized){
        tuya_mqtt_deinit(client);
    }
    ubus_free(ctx);
    return ret;
}
 
 
 