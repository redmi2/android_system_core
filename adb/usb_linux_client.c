/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include "sysdeps.h"

#define   TRACE_TAG  TRACE_USB
#include "adb.h"

#define USB_FUNCTIONS_PATH     "/sys/class/android_usb/android0/functions"
#define USB_ENABLE_PATH        "/sys/class/android_usb/android0/enable"
#define USB_PID_PATH           "/sys/class/android_usb/android0/idProduct"

int function_enabled(char *match)
{
    int fd, ret;
    char c[256];

    fd = unix_open(USB_FUNCTIONS_PATH, O_RDONLY);
    if(fd < 0) {
        D("Error while opening the file %s : %s \n",
             USB_FUNCTIONS_PATH, strerror(errno));
        return 0;
    }

    if(unix_read(fd, c, sizeof(c)) < 0) {
        D("Error while reading the file %s : %s \n",
             USB_FUNCTIONS_PATH, strerror(errno));
        ret = 0;
    }

    if(strstr(c, match)) {
        ret = 1;
    } else {
        ret = 0;
    }

    unix_close(fd);
    return ret;
}

void usb_adb_enable(int enable)
{
    int fd, fd_enable;
    char *pid, *funcs;

    if(enable == function_enabled("adb"))
        return;

    if(function_enabled("rndis")) {
        if(enable) {
            pid = "0x9024";
            funcs = "rndis,adb";
        } else {
            pid = "0xf00e";
            funcs = "rndis";
        }
    } else {
        if(enable) {
            pid = "0x9025";
            funcs = "diag,adb,serial,rmnet,mass_storage";
        } else {
            pid = "0x9026";
            funcs = "diag,serial,rmnet,mass_storage";
        }
    }
    D("Enabling funcs:%s, pid:%s\n", funcs, pid);

    fd_enable = unix_open(USB_ENABLE_PATH, O_WRONLY);
    if(fd_enable < 0) {
        D("Error while opening the file %s : %s \n",
             USB_ENABLE_PATH, strerror(errno));
        return;
    }
    if(unix_write(fd_enable, "0", 2) < 0) {
        D("Error while reading the file %s : %s \n",
             USB_ENABLE_PATH, strerror(errno));
        unix_close(fd_enable);
        return;
    }

    fd = unix_open(USB_PID_PATH, O_WRONLY);
    if(fd < 0) {
        D("Error while opening the file %s : %s \n",
             USB_PID_PATH, strerror(errno));
        unix_close(fd_enable);
        return;
    }
    if(unix_write(fd, pid, strlen(pid) + 1) < 0) {
        D("Error while reading the file %s : %s \n",
             USB_PID_PATH, strerror(errno));
        unix_close(fd);
        unix_close(fd_enable);
        return;
    }
    unix_close(fd);

    fd = unix_open(USB_FUNCTIONS_PATH, O_WRONLY);
    if(fd < 0) {
        D("Error while opening the file %s : %s \n",
             USB_FUNCTIONS_PATH, strerror(errno));
        unix_close(fd_enable);
        return;
    }
    if(unix_write(fd, funcs, strlen(funcs) + 1) < 0) {
        D("Error while reading the file %s : %s \n",
             USB_PID_PATH, strerror(errno));
        unix_close(fd);
        unix_close(fd_enable);
        return;
    }
    unix_close(fd);

    if(unix_write(fd_enable, "1", 2) < 0) {
        D("Error while reading the file %s : %s \n",
             USB_ENABLE_PATH, strerror(errno));
        unix_close(fd_enable);
        return;
    }

    unix_close(fd_enable);
}

static void sigterm_handler(int n)
{
    usb_adb_enable(0);
    exit(EXIT_SUCCESS);
}

struct usb_handle
{
    int fd;
    adb_cond_t notify;
    adb_mutex_t lock;
};

void usb_cleanup()
{
    // nothing to do here
}

static void *usb_open_thread(void *x)
{
    struct usb_handle *usb = (struct usb_handle *)x;
    int fd;

    signal(SIGTERM, sigterm_handler);
    while (1) {
        // wait until the USB device needs opening
        adb_mutex_lock(&usb->lock);
        while (usb->fd != -1)
            adb_cond_wait(&usb->notify, &usb->lock);
        adb_mutex_unlock(&usb->lock);

        D("[ usb_thread - opening device ]\n");
        do {
            /* XXX use inotify? */
            fd = unix_open("/dev/android_adb", O_RDWR);
            if (fd < 0) {
                // to support older kernels
                fd = unix_open("/dev/android", O_RDWR);
            }
            if (fd < 0) {
                adb_sleep_ms(1000);
            }
        } while (fd < 0);
        D("[ opening device succeeded ]\n");

        close_on_exec(fd);
        usb->fd = fd;

        D("[ usb_thread - registering device ]\n");
        register_usb_transport(usb, 0, 1);
    }

    // never gets here
    return 0;
}

int usb_write(usb_handle *h, const void *data, int len)
{
    int n;

    D("[ write %d ]\n", len);
    n = adb_write(h->fd, data, len);
    if(n != len) {
        D("ERROR: n = %d, errno = %d (%s)\n",
            n, errno, strerror(errno));
        return -1;
    }
    D("[ done ]\n");
    return 0;
}

int usb_read(usb_handle *h, void *data, int len)
{
    int n;

    D("[ read %d ]\n", len);
    n = adb_read(h->fd, data, len);
    if(n != len) {
        D("ERROR: n = %d, errno = %d (%s)\n",
            n, errno, strerror(errno));
        return -1;
    }
    return 0;
}

void usb_init()
{
    usb_handle *h;
    adb_thread_t tid;
    int fd;

    h = calloc(1, sizeof(usb_handle));
    h->fd = -1;
    adb_cond_init(&h->notify, 0);
    adb_mutex_init(&h->lock, 0);

    // Open the file /dev/android_adb_enable to trigger 
    // the enabling of the adb USB function in the kernel.
    // We never touch this file again - just leave it open
    // indefinitely so the kernel will know when we are running
    // and when we are not.
    fd = unix_open("/dev/android_adb_enable", O_RDWR);
    if (fd < 0) {
       D("failed to open /dev/android_adb_enable\n");
       //Also check if new framework is supported
       usb_adb_enable(1);
    } else {
        close_on_exec(fd);
    }

    D("[ usb_init - starting thread ]\n");
    if(adb_thread_create(&tid, usb_open_thread, h)){
        fatal_errno("cannot create usb thread");
    }
}

void usb_kick(usb_handle *h)
{
    D("usb_kick\n");
    adb_mutex_lock(&h->lock);
    adb_close(h->fd);
    h->fd = -1;

    // notify usb_open_thread that we are disconnected
    adb_cond_signal(&h->notify);
    adb_mutex_unlock(&h->lock);
}

int usb_close(usb_handle *h)
{
    // nothing to do here
    return 0;
}
