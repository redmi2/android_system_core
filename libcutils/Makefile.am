
# NOTE: Only supports builds of shared/static libraries for the target

common_sources := array.c
common_sources += hashmap.c
common_sources += atomic.c
common_sources += native_handle.c
common_sources += buffer.c
common_sources += socket_inaddr_any_server.c
common_sources += socket_local_client.c
common_sources += socket_local_server.c
common_sources += socket_loopback_client.c
common_sources += socket_loopback_server.c
common_sources += socket_network_client.c
common_sources += config_utils.c
common_sources += cpu_info.c
common_sources += load_file.c
common_sources += strdup16to8.c
common_sources += strdup8to16.c
common_sources += record_stream.c
common_sources += process_name.c
common_sources += properties.c
common_sources += threads.c
common_sources += sched_policy.c

# TODO: When building against Mingw, these files should NOT be built
common_sources += abort_socket.c
common_sources += mspace.c
common_sources += selector.c
common_sources += tztime.c
common_sources += adb_networking.c
common_sources += zygote.c

# Target libcutils
lib_LTLIBRARIES := libcutils.la

libcutils_la_CPPFLAGS := -I../include
libcutils_la_CPPFLAGS += -DHAVE_PTHREADS
libcutils_la_CPPFLAGS += -DLINUX_ENABLED

libcutils_la_LDFLAGS := -version-info 1:0:0
libcutils_la_LIBADD  := -lpthread

libcutils_la_SOURCES := $(common_sources)
libcutils_la_SOURCES += ashmem-dev.c
libcutils_la_SOURCES += mq.c

if ARM
libcutils_la_SOURCES += memset32.S

if !HAVE_STRLCPY
libcutils_la_SOURCES += memory.c

# If both memset32.S and memory.c are compiled and linked, there's going to be
# multiple definition erros when linking libcutils.so because both files define
# 'android_memset16' and 'android_memset32'. Therefore, strip out the symbols
# from memory.o before linking libcutils.so
libcutils_la_DEPENDENCIES := strip-libcutils_la-memory.lo
strip-libcutils_la-memory.lo: symbolnames := android_memset16 android_memset32

strip-%.lo: %.lo
	@echo "Stripping $*.lo's object files..."
	@if test -n "$(symbolnames)"; then \
		test ! -e $*.o       || (echo "Stripping '$(symbolnames)' from '$*.o'";       $(STRIP) $(addprefix -N ,$(symbolnames)) $*.o); \
		test ! -e .libs/$*.o || (echo "Stripping '$(symbolnames)' from '.libs/$*.o'"; $(STRIP) $(addprefix -N ,$(symbolnames)) .libs/$*.o); \
	else \
		echo "ERROR: No symbols given to strip from $*.lo's object files!"; \
		false; \
	fi
	@touch $@
endif # !HAVE_STRLCPY

else  # !ARM
libcutils_la_SOURCES += memory.c
endif # !ARM

