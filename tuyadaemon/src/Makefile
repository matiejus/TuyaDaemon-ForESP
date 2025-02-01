TARGET = tuya_daemon
SRC:=$(wildcard *.c)
OBJS = $(SRC:.c=.o)

# Compiler flags
CFLAGS := -g -Wall

# Linker flags (add library paths and libraries to link against)
LDFLAGS := -llink_core -lmiddleware_implementation -lplatform_port -lutils_modules -largp -lubus -lubox

.PHONY: all clean

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -r $(OBJS) $(TARGET)