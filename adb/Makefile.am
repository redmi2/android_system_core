bin_PROGRAMS = adbd

adbd_CC = @CC@

adbd_SOURCES = adb.c \
	fdevent.c \
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

AM_CFLAGS = -O2 \
	-g \
	-DADB_HOST=0 \
	-Wall \
	-Wno-unused-parameter \
	-D_XOPEN_SOURCE \
	-D_GNU_SOURCE \
	-DANDROID_GADGET=1 \
	-DLINUX_ENABLED=1 \
	-I../include \
	-I../../kernel/include/linux \
	-include ../include/arch/linux-arm/OEConfig.h

adbd_LDFLAGS = -lpthread -all-static

adbd_LDADD =  ../libcutils/libcutils.a
