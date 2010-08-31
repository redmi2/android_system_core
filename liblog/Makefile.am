
# NOTE: Only supports builds of shared/static libraries for the target

liblog_sources = logd_write.c \
			logprint.c \
			event_tag_map.c

AM_CPPFLAGS = -I../include

lib_LTLIBRARIES = liblog.la
liblog_la_SOURCES = $(liblog_sources)
liblog_la_LIBADD = -lpthread
liblog_la_LDFLAGS = -version-info 1:0:0
