lib_LTLIBRARIES := libzipfile.la

libzipfile_la_CFLAGS := -O2 -g
libzipfile_la_CFLAGS += -I../include
libzipfile_la_CFLAGS += -include ../include/arch/linux-arm/OEConfig.h

libzipfile_la_LDFLAGS = -lpthread -lz

libzipfile_la_SOURCES = centraldir.c zipfile.c

