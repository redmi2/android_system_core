
SRCDIR := $(TOPDIR)/system/core

vpath %.c $(SRCDIR)/adb

ADB_SRCLIST := \
	adb.c \
	transport.c \
	transport_local.c \
	transport_usb.c \
	sockets.c \
	services.c \
	file_sync_service.c \
	jdwp_service.c \
	framebuffer_service.c \
	remount_service.c \
	usb_linux_client.c \
	log_service.c \
	utils.c

ADB_OBJLIST := $(ADB_SRCLIST:%.c=%.o)

ADB_CFLAGS := -O2 -g -DADB_HOST=0 -Wall -Wno-unused-parameter
ADB_CFLAGS += -D_XOPEN_SOURCE -D_GNU_SOURCE -DANDROID_GADGET=1 -DLINUX_ENABLED=1
ADB_CFLAGS += -I$(SRCDIR)/adb -I$(SRCDIR)/include
LIB_FLAGS += -lpthread -L./ -L$(SYSROOTLIB_DIR) -lcutils

all: adbd
adbd: $(ADB_OBJLIST)
	$(CC) $^ -o adbd $(LIB_FLAGS)

%.o: %.c
	$(CC) $(ADB_CFLAGS) -c $^ -o $@

clean:
	rm -rf *.o adbd  *.a
