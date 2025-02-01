#include "ubus_logic.h"

static const struct blobmsg_policy memory_policy[__MEMORY_MAX] = {
	[TOTAL_MEMORY]	  = { .name = "total", .type = BLOBMSG_TYPE_INT64 },
	[FREE_MEMORY]	  = { .name = "free", .type = BLOBMSG_TYPE_INT64 },
	[SHARED_MEMORY]	  = { .name = "shared", .type = BLOBMSG_TYPE_INT64 },
	[BUFFERED_MEMORY] = { .name = "buffered", .type = BLOBMSG_TYPE_INT64 },
};

static const struct blobmsg_policy info_policy[__INFO_MAX] = {
	[MEMORY_DATA] = { .name = "memory", .type = BLOBMSG_TYPE_TABLE },
};

void board_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	struct MemData *memoryData = (struct MemData *)req->priv;
	struct blob_attr *tb[__INFO_MAX];
	struct blob_attr *memory[__MEMORY_MAX];

	blobmsg_parse(info_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[MEMORY_DATA]) {
		syslog(LOG_ERR, "No memory data received\n");
		return;
	}

	blobmsg_parse(memory_policy, __MEMORY_MAX, memory, blobmsg_data(tb[MEMORY_DATA]),blobmsg_data_len(tb[MEMORY_DATA]));

    if (!memory[TOTAL_MEMORY] || !memory[FREE_MEMORY] || !memory[SHARED_MEMORY] || !memory[BUFFERED_MEMORY]) {
		syslog(LOG_ERR, "No memory data received\n");
		return;
	}

	memoryData->total    = blobmsg_get_u64(memory[TOTAL_MEMORY]);
	memoryData->free     = blobmsg_get_u64(memory[FREE_MEMORY]);
	memoryData->shared   = blobmsg_get_u64(memory[SHARED_MEMORY]);
	memoryData->buffered = blobmsg_get_u64(memory[BUFFERED_MEMORY]);
}

int init_ubus()
{
	ctx = ubus_connect(NULL);
	if (!ctx) {
		fprintf(stderr, "Failed to connect to ubus\n");
        syslog(LOG_ERR, "Failed to connect to ubus\n");
		return 1;
	}
	return 0;
}

char* collect_memory_info()
{
	uint32_t id;

	struct MemData memory = { 0 };

	if (ubus_lookup_id(ctx, "system", &id) ||
	    ubus_invoke(ctx, id, "info", NULL, board_cb, &memory, 3000)) {
		syslog(LOG_ERR, "cannot request memory info from procd\n");
        return NULL;
	}

    cJSON* root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "total_ram",  memory.total);
    cJSON_AddNumberToObject(root, "free_ram",  memory.free);
    cJSON_AddNumberToObject(root, "shared_ram",memory.shared);
    cJSON_AddNumberToObject(root, "buffered_ram",memory.buffered);
    char* payload = cJSON_PrintUnformatted(root);

	cJSON_free(root);

    syslog(LOG_INFO,"Sending payload: %s\n", payload);

	return payload;
}