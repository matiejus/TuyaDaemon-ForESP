#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <syslog.h>
#include <cJSON.h>

struct MemData {
	int total;
	int free;
	int shared;
	int buffered;
};

enum {
	TOTAL_MEMORY,
	FREE_MEMORY,
	SHARED_MEMORY,
	BUFFERED_MEMORY,
	__MEMORY_MAX,
};

enum {
	MEMORY_DATA,
	__INFO_MAX,
};

struct ubus_context *ctx;

void board_cb(struct ubus_request *req, int type, struct blob_attr *msg);

int init_ubus();

char* collect_memory_info();