AM_CPPFLAGS := \
              -I. \
              -I../include \
              -I../ext4_utils \
              -I./include \
              -Dstrlcat=strncat

c_sources := fs_mgr.c fs_mgr_verity.c

library_includedir := $(includedir)
library_include_HEADERS := include/fs_mgr.h

lib_LIBRARIES := libfs_mgr.a
libfs_mgr_a_SOURCES := $(c_sources)
libfs_mgr_a_CFLAGS := $(AM_CFLAGS)
libfs_mgr_a_CPPFLAGS := $(AM_CPPFLAGS)
libfs_mgr_a_LDFLAGS := -lext4_utils -lrt
