
vpath %.c $(SRCDIR)/libcutils
vpath %.S $(SRCDIR)/libcutils

LIBCUTILS_SRCLIST := \
	array.c \
	hashmap.c \
	atomic.c \
	buffer.c \
	socket_inaddr_any_server.c \
	socket_local_client.c \
	socket_local_server.c \
	socket_loopback_client.c \
	socket_loopback_server.c \
	socket_network_client.c \
	config_utils.c \
	cpu_info.c \
	load_file.c \
	strdup16to8.c \
	strdup8to16.c \
	record_stream.c \
	process_name.c \
	properties.c \
	threads.c \
        mspace.c \
        selector.c \
        fdevent.c \
        tztime.c \
        tzstrftime.c \
        adb_networking.c \
	zygote.c \
	ashmem-dev.c \
	mq.c \
	memset32.S \
	atomic-android-arm.S

LIBCUTILS_OBJS := $(LIBCUTILS_SRCLIST:%.c=%.o)

LIBCUTILS_CFLAGS := -I$(SRCDIR)/include -I$(SRCDIR)/libcutils -I$(SRCDIR)/liblog -DHAVE_PTHREADS -DLINUX_ENABLED=1

all: libcutils.a

libcutils.a: $(LIBCUTILS_OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(LIBCUTILS_CFLAGS) -c $^ -o $@

clean:
	rm -rf *.o *.a

