
# NOTE: Only supports builds of shared/static libraries for the target

libcutils_sources = array.c \
			hashmap.c \
			atomic.c \
			native_handle.c \
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
			sched_policy.c \
			abort_socket.c \
			mspace.c \
			selector.c \
			tztime.c \
			adb_networking.c \
			zygote.c \
			ashmem-dev.c \
			mq.c \
			memset32.S

AM_CPPFLAGS = -I../include \
		-DHAVE_PTHREADS \
		-DLINUX_ENABLED=1

lib_LTLIBRARIES = libcutils.la
libcutils_la_SOURCES = $(libcutils_sources)
libcutils_la_LDFLAGS = -version-info 1:0:0
libcutils_la_LIBADD = ../liblog/liblog.la
