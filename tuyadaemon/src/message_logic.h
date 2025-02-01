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
//#include "tuya_log.h" // Teko atsikratyti, nes naudoja tuya loginima ir pykstasi su syslog
#include "tuya_error_code.h"
#include "system_interface.h"
#include "mqtt_client_interface.h"
#include "tuyalink_core.h"

char* get_action_code(const char *json_string);

char* decode_json_to_string(cJSON* cjson,const char* data);

cJSON* decode_json(const char* data_string,const char* data);

int decode_json_to_int(cJSON* cjson,const char* data);

int data_string_to_int(const char* data_string,const char* key);

void write_to_file(const char *data_string);
