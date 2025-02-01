#include "tuya_logic.h"
#include "tuya_cacert.h"

int tuya_main(struct arguments* arguments,tuya_mqtt_context_t* client)
{   
    int ret = OPRT_OK;

    ret = tuya_mqtt_init(client, &(const tuya_mqtt_config_t) {
        .host = "m1.tuyacn.com",
        .port = 8883,
        .cacert = tuya_cacert_pem,
        .cacert_len = sizeof(tuya_cacert_pem),
        .device_id = arguments->deviceId,
        .device_secret = arguments->deviceSecret,
        .keepalive = 100,
        .timeout_ms = 2000,
        .on_connected = on_connected,
        .on_disconnect = on_disconnect,
        .on_messages = on_messages
    });
    if (ret != OPRT_OK) {
        fprintf(stderr, "ERROR: Could not initialize MQTT (code %d)\n", ret);
        syslog(LOG_ERR,"ERROR: Could not initialize MQTT (code %d)\n", ret);
        return 2;
    }
 
    ret = tuya_mqtt_connect(client);
    
    if (ret != OPRT_OK) {
        fprintf(stderr, "Error: Could not connect to Tuya (code %d)\n", ret);
        syslog(LOG_ERR,"Error: Could not connect to Tuya (code %d)\n", ret);
        return 3;
    }
}

void tuya_loop(tuya_mqtt_context_t* client)
{   
    int counter = 0;
    for (;;) {
        if(flag==1){
            tuya_mqtt_disconnect(client);
            tuya_mqtt_connect(client);
            flag = 0;
        }
        if (counter>30) { //around minute cycle to send sysinfo
            counter=0;
            send_sysinfo(client);
            
        }
        /* Loop to receive packets, and handles client keepalive */
        counter++;
        tuya_mqtt_loop(client);
    }
}

void send_sysinfo(tuya_mqtt_context_t* client)
{
	char *payload = collect_memory_info();
 
    int ret = tuyalink_thing_property_report_with_ack(client, client->config.device_id, payload);
    if (ret <= 0) {
        syslog(LOG_ERR,"Failed to send sysinfo: %d\n", ret);
        if(ret == OPRT_INVALID_PARM){
            syslog(LOG_ERR,"Invalid operands: %d\n", ret);
        }
        if(ret == OPRT_MALLOC_FAILED){
            syslog(LOG_ERR,"Malloc failed: %d\n", ret);
        }
        if(ret == OPRT_LINK_CORE_MQTT_PUBLISH_ERROR){
            syslog(LOG_ERR,"MQTT failed to publish: %d\n", ret);
        }
    } else {
        syslog(LOG_INFO,"Successfully sent sysinfo\n");
    }

    free(payload);
}

void send_random_value(tuya_mqtt_context_t* client)
{
    int random_number = rand() % (10001);
    cJSON* root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "random_int", random_number);
    char* payload = cJSON_PrintUnformatted(root);
 
    syslog(LOG_INFO,"Sending payload: %s\n", payload);
 
    int ret = tuyalink_thing_property_report_with_ack(client, client->config.device_id, payload);
    if (ret <= 0) {
        syslog(LOG_ERR,"Failed to send random value: %d\n", ret);
        if(ret == OPRT_INVALID_PARM){
            syslog(LOG_ERR,"Invalid operands: %d\n", ret);
        }
        if(ret == OPRT_MALLOC_FAILED){
            syslog(LOG_ERR,"Malloc failed: %d\n", ret);
        }
        if(ret == OPRT_LINK_CORE_MQTT_PUBLISH_ERROR){
            syslog(LOG_ERR,"MQTT failed to publish: %d\n", ret);
        }
    } else {
        syslog(LOG_INFO,"Successfully sent RANDOM value: %d\n", random_number);
    }
    
    cJSON_Delete(root);
    free(payload);
}

void on_connected(tuya_mqtt_context_t* context, void* user_data)
{
    syslog(LOG_INFO, "Succesfuly connected to the cloud host");
}
 
void on_disconnect(tuya_mqtt_context_t* context, void* user_data)
{
    syslog(LOG_INFO, "Disconnected from the cloud host");
}
 
void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg)
{
    syslog(LOG_INFO,"on message id:%s, type:%d, code:%d", msg->msgid, msg->type, msg->code);
    switch (msg->type) {
        case THING_TYPE_ACTION_EXECUTE:
            syslog(LOG_INFO,"Receiving action: %s\n",  msg->data_string);
            char* action = get_action_code(msg->data_string);
            syslog(LOG_INFO,"Decoded action: %s\n", action);
            if (strcmp(action, "receive_all_vars") == 0) {
                send_random_value(context);
                send_sysinfo(context);
            }
            else if (strcmp(action, "tuya_action") == 0) {
                write_to_file(msg->data_string);
            }
            else {
                syslog(LOG_ERR, "Unknown action: %s\n", action);
            }
            free(action);
            break;

        default:
            break;
    }
    printf("\r\n");
}