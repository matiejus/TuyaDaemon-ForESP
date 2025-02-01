#include "message_logic.h"

char* get_action_code(const char *json_string) {
    
    cJSON *json = cJSON_Parse(json_string);
    if (json == NULL) {
        syslog(LOG_ERR,"Error parsing JSON\n");
        return NULL;
    }

    cJSON *actionCode = cJSON_GetObjectItemCaseSensitive(json, "actionCode");
    
    if (!cJSON_IsString(actionCode) || (actionCode->valuestring == NULL)) {
        syslog(LOG_ERR,"Invalid JSON format: actionCode is not a string\n");
        cJSON_Delete(json);
        return NULL;
    }

    // Allocate memory for the action code string
    char *action_code_str = strdup(actionCode->valuestring);

    cJSON_Delete(json);
    return action_code_str;
}
 
int data_string_to_int(const char* data_string,const char* key)
{   
    cJSON* cjson = cJSON_Parse(data_string);
    
    int int_value =decode_json_to_int(cjson,key);

    syslog(LOG_INFO, "Succesfully decoded int value");
    cJSON_Delete(cjson);
    return int_value;
}

char* decode_json_to_string(cJSON* cjson,const char* data)
{
    syslog(LOG_INFO,"decode_json data: %s\n",data);
    
    cJSON* decoded_data = cJSON_GetObjectItem(cjson, data);
    if(decoded_data == NULL) {
        syslog(LOG_ERR,"Failed to decode json to string, no value under this key");
        return NULL;
    }
    syslog(LOG_INFO,"Succesfully decoded json");

    char *decoded_string = strdup(decoded_data->valuestring);

    return decoded_string;
}

int decode_json_to_int(cJSON* cjson,const char* data)
{
    syslog(LOG_INFO,"decode_json data: %s\n",data);

    char* json_string = cJSON_Print(cjson);
    syslog(LOG_INFO, "Parsed JSON to string: %s\n", json_string);

    cJSON* decoded_data = cJSON_GetObjectItem(cjson, data);
    if(decoded_data == NULL) {
        free(json_string);
        syslog(LOG_ERR,"Failed to decode json to int, no value under this key");
        return -1;
    }
    syslog(LOG_INFO,"Succesfully decoded json to int");
    
    free(json_string);
    return decoded_data->valueint;
}

cJSON* decode_json(const char* data_string,const char* data)
{
    cJSON* root = cJSON_Parse(data_string);
    if (root == NULL) {
        syslog(LOG_ERR, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return NULL;
    }
    
    char* json_string = cJSON_Print(root);
    syslog(LOG_INFO, "Parsed JSON to string: %s\n", json_string);
    
    cJSON* decoded_data = cJSON_GetObjectItem(root, data);
    if(decoded_data == NULL) {
        free(json_string);
        syslog(LOG_ERR,"Failed to decode json to sub json, no value under this key");
        return NULL;
    }
    json_string = cJSON_Print(decoded_data);
    syslog(LOG_INFO,"Succesfully decoded json");

    free(json_string);
    return decoded_data;
}

void write_to_file(const char *data_string) 
{
    const char *filename = "/tmp/tuya_action.log";

    syslog(LOG_INFO,"Sending data_string %s\n", data_string);
    cJSON* data = decode_json(data_string,"inputParams");
    char* temp_message = decode_json_to_string(data,"temp_message");

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        syslog(LOG_ERR,"Error opening file");
        return;
    }

    syslog(LOG_INFO,"Sending message to /tmp/tuya_action.log %s\n",temp_message);
    fprintf(file, "%s\n", temp_message);

    if (fclose(file) != 0) {
        perror("Error closing file");
    }
}
