#include "cJSON.h"
//#include "tuya_log.h" // Teko atsikratyti, nes naudoja tuya loginima ir pykstasi su syslog
#include "tuya_error_code.h"
#include "system_interface.h"
#include "mqtt_client_interface.h"
#include "tuyalink_core.h"
#include "message_logic.h"
#include "ubus_logic.h"

struct arguments {    
    char productId[17];
    char deviceId[23];
    char deviceSecret[17];
    bool foreground;
};

volatile sig_atomic_t flag;

volatile sig_atomic_t mqtt_initialized;

tuya_mqtt_context_t client_instance;

int tuya_main(struct arguments* arguments,tuya_mqtt_context_t* client);

void tuya_loop(tuya_mqtt_context_t* client);

void send_random_value(tuya_mqtt_context_t* client);

void send_sysinfo(tuya_mqtt_context_t* client);

void on_connected(tuya_mqtt_context_t* context, void* user_data);
 
void on_disconnect(tuya_mqtt_context_t* context, void* user_data);
 
void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg);