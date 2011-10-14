/*
 * Copyright (C) 2007 The Android Open Source Project
 * Copyright (C) 2011, Code Aurora Forum. All rights reserved.
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

struct usb_target_pid_table
{
    const char *platform;
    const char *baseband;
    const char *pid;
    const char *functions;
};

//List PIDs with RNDIS enabled; ADB disabled
struct usb_target_pid_table enableRNDIS_disableADB_list[] = {
    { NULL,      "csfb",   "0x9041", "rndis,diag", },
    { NULL,      "svlte2", "0x9041", "rndis,diag", },
    { NULL,       NULL,    "0xf00e", "rndis", },      //default PID
};

//List PIDs with both RNDIS and ADB enabled
struct usb_target_pid_table enableRNDIS_enableADB_list[] = {
    { NULL,      "csfb",   "0x9042", "rndis,diag,adb" },
    { NULL,      "svlte2", "0x9042", "rndis,diag,adb" },
    { NULL,       NULL,    "0x9024", "rndis,adb" },  //default PID
};

//List PIDs with RNDIS disabled; ADB enabled
struct usb_target_pid_table disableRNDIS_enableADB_list[] = {
    { "msm8960",  NULL,    "0x9025", "diag,adb,serial,rmnet,mass_storage" },
    { NULL,      "csfb",   "0x9031", "diag,adb,serial,rmnet_sdio,mass_storage" },
    { NULL,      "svlte2", "0x9037", "diag,adb,serial,rmnet_smd_sdio,mass_storage" },
    { NULL,       NULL,    "0x9025", "diag,adb,serial,rmnet_smd,mass_storage" },
};

//List PIDs with both RNDIS and ADB disabled
struct usb_target_pid_table disableRNDIS_disableADB_list[] = {
    { "msm8960",  NULL,    "0x9026", "diag,serial,rmnet,mass_storage" },
    { NULL,      "csfb",   "0x9032", "diag,serial,rmnet_sdio,mass_storage" },
    { NULL,      "svlte2", "0x9038", "diag,serial,rmnet_smd_sdio,mass_storage" },
    { NULL,       NULL,    "0x9026", "diag,serial,rmnet_smd,mass_storage" },
};


int function_enabled(char *match)
{
    int fd, ret, n_read;
    char c[256];

    fd = unix_open(USB_FUNCTIONS_PATH, O_RDONLY);
    if(fd < 0) {
        D("Error while opening the file %s : %s \n",
             USB_FUNCTIONS_PATH, strerror(errno));
        return 0;
    }

    n_read = unix_read(fd, c, sizeof(c) - 1);
    if(n_read < 0) {
        D("Error while reading the file %s : %s \n",
             USB_FUNCTIONS_PATH, strerror(errno));
        unix_close(fd);
        return 0;
    }

    c[n_read] = '\0';
    if(strstr(c, match)) {
        ret = 1;
    } else {
        ret = 0;
    }

    unix_close(fd);
    return ret;
}

void select_pid_funcs(const char **pid, const char **funcs, int adb_enable)
{
    char target[PROPERTY_VALUE_MAX];
    char baseband[PROPERTY_VALUE_MAX];
    int rndis_enable;
    int count, i;
    struct usb_target_pid_table *pid_table;

    property_get("ro.board.platform", target, "");
    property_get("ro.baseband", baseband, "");

    rndis_enable = function_enabled("rndis");

    if(rndis_enable == 1 && adb_enable == 1) {
        pid_table = enableRNDIS_enableADB_list;
        count = sizeof(enableRNDIS_enableADB_list);
    } else if(rndis_enable == 1 && adb_enable == 0) {
        pid_table = enableRNDIS_disableADB_list;
        count = sizeof(enableRNDIS_disableADB_list);
    } else if(rndis_enable == 0 && adb_enable == 1) {
        pid_table = disableRNDIS_enableADB_list;
        count = sizeof(disableRNDIS_enableADB_list);
    } else {
        pid_table = disableRNDIS_disableADB_list;
        count = sizeof(disableRNDIS_disableADB_list);
    }

    for( i = 0; i < count; i++)
    {
        if((((pid_table[i].platform == NULL)) ||
                (!strncmp(pid_table[i].platform, target, PROPERTY_VALUE_MAX))) &&
           ((pid_table[i].baseband == NULL) ||
                (!strncmp(pid_table[i].baseband, baseband, PROPERTY_VALUE_MAX)))) {
            *pid = pid_table[i].pid;
            *funcs = pid_table[i].functions;
            return;
        }
    }
    //shouldn't reach this point
    D("Error while locating PID for device:%s, basebad:%s\n",
             target, baseband);
    *pid = "";
    *funcs = "";

    return;
}

int is_usb_enable(void)
{
    int fd_enable, ret, n_read;
    char c[16];

    fd_enable = unix_open(USB_ENABLE_PATH, O_RDONLY);
    if(fd_enable < 0) {
        D("Error while opening the file %s : %s \n",
             USB_ENABLE_PATH, strerror(errno));
        return 0;
    }

    n_read = unix_read(fd_enable, c, sizeof(c) - 1);
    if(n_read < 0) {
        D("Error while reading the file %s : %s \n",
             USB_ENABLE_PATH, strerror(errno));
        unix_close(fd_enable);
        return 0;
    }

    c[n_read] = '\0';
    if(strstr(c, "1")) {
        ret = 1;
    } else {
        ret = 0;
    }

    unix_close(fd_enable);
    return ret;
}

void usb_adb_enable(int enable)
{
    int fd, fd_enable;
    const char *pid, *funcs;

    if (!is_usb_enable())
        return;

    if(enable == function_enabled("adb"))
        return;

    select_pid_funcs(&pid, &funcs, enable);

    D("Enabling USB funcs:%s, pid:%s\n", funcs, pid);

    fd_enable = unix_open(USB_ENABLE_PATH, O_WRONLY);
    if(fd_enable < 0) {
        D("Error while opening the file %s : %s \n",
             USB_ENABLE_PATH, strerror(errno));
        return;
    }
    if(unix_write(fd_enable, "0", 2) < 0) {
        D("Error while writing to the file %s : %s \n",
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
        D("Error while writing to the file %s : %s \n",
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
        D("Error while writing to the file %s : %s \n",
             USB_FUNCTIONS_PATH, strerror(errno));
        unix_close(fd);
        unix_close(fd_enable);
        return;
    }
    unix_close(fd);

    if(unix_write(fd_enable, "1", 2) < 0) {
        D("Error while writing to the file %s : %s \n",
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
