/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
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

#include <errno.h>
#include <fcntl.h>

#include <sys/mount.h>

#include "vold.h"
#include "volmgr.h"
#include "volmgr_vfat.h"
#include "logwrapper.h"

#define VFAT_DEBUG 0
#define VFAT_TYPE_LEN 9
#define VFAT_VOL_NAME_LEN 12

static char FSCK_MSDOS_PATH[] = "/system/bin/dosfsck";

int vfat_identify(blkdev_t *dev, char **volume_name)
{
    int rc = -1, fd;
    char *devpath, vfat_type [VFAT_TYPE_LEN], vfat_volume_name[VFAT_VOL_NAME_LEN];
    int i = 0;

#if VFAT_DEBUG
    LOG_VOL("vfat_identify(%d:%d):", dev->major, dev->minor);
#endif

    devpath = blkdev_get_devpath(dev);

    if ((fd = open(devpath, O_RDWR)) < 0) {
        LOGE("Unable to open device '%s' (%s)", devpath,
             strerror(errno));
        free(devpath);
        return -errno;
    }

    /* For FAT16:
     * The fat type is stored at byte 55 and is 8 bytes long
     * The volume label/name is stored at byte 44 and is 11 bytes long
     */

    if (lseek(fd, 54, SEEK_SET) < 0) {
        LOGE("Unable to lseek to get superblock (%s)", strerror(errno));
        rc =  -errno;
        goto out;
    }
    /* Read FAT type */
    if (read(fd, &vfat_type, sizeof(vfat_type)) != sizeof(vfat_type)) {
        LOGE("Unable to read superblock (%s)", strerror(errno));
        rc =  -errno;
        goto out;
    }

    vfat_type[VFAT_TYPE_LEN - 1] = '\0';
    /* Check FAT type for FAT16 */
    if (!strncmp(vfat_type,"FAT16",5)) {
        if (lseek(fd, 43, SEEK_SET) < 0) {
            LOGE("Unable to lseek to get superblock (%s)", strerror(errno));
            rc =  -errno;
            goto out;
        }
        /* Read volume name */
        if (read(fd, vfat_volume_name, sizeof(vfat_volume_name)) != sizeof(vfat_volume_name)) {
            LOGE("Unable to read superblock (%s)", strerror(errno));
            rc =  -errno;
            goto out;
        }

        vfat_volume_name[VFAT_VOL_NAME_LEN - 1] = '\0';
        rc = 0;
        goto trim;
    }

    /* For FAT32:
     * The fat type is stored at byte 83 and is 8 bytes long
     * The volume label/name is stored at byte 72 and is 11 bytes long
     */
    if (lseek(fd, 82, SEEK_SET) < 0) {
        LOGE("Unable to lseek to get superblock (%s)", strerror(errno));
        rc =  -errno;
        goto out;
    }
    /* Read File type info */
    if (read(fd, &vfat_type, sizeof(vfat_type)) != sizeof(vfat_type)) {
        LOGE("Unable to read superblock (%s)", strerror(errno));
        rc =  -errno;
        goto out;
    }

    vfat_type[VFAT_TYPE_LEN - 1] = '\0';
    /* Check file type for FAT32 */
    if (!strncmp(vfat_type,"FAT32",5)) {
        if (lseek(fd, 71, SEEK_SET) < 0) {
            LOGE("Unable to lseek to get superblock (%s)", strerror(errno));
            rc =  -errno;
            goto out;
        }

     if (read(fd, vfat_volume_name, sizeof(vfat_volume_name)) != sizeof(vfat_volume_name)) {
         LOGE("Unable to read superblock (%s)", strerror(errno));
         rc =  -errno;
         goto out;
        }

     vfat_volume_name[VFAT_VOL_NAME_LEN -1] = '\0';
     rc = 0;
    }

trim:
    /*
     * Remove blank space at the end of the string, volume name
     */
    for (i = (strlen(vfat_volume_name)-1); i >= 0; i--) {
        if (vfat_volume_name[i] == ' ') {
            vfat_volume_name[i] = '\0';
        } else {
            break;
        }
    }
    if (strlen(vfat_volume_name))
        *volume_name = strdup(vfat_volume_name);
    else
        *volume_name = NULL;
    LOGE("Volume_name= %s", vfat_volume_name);

out:
#if VFAT_DEBUG
    LOG_VOL("vfat_identify(%s): rc = %d", devpath, rc);
#endif
    free(devpath);
    close(fd);
    return rc;
}

int vfat_check(blkdev_t *dev)
{
    int rc;

#if VFAT_DEBUG
    LOG_VOL("vfat_check(%d:%d):", dev->major, dev->minor);
#endif

    if (access(FSCK_MSDOS_PATH, X_OK)) {
        LOGE("vfat_check(%d:%d): %s not found (skipping checks)",
             dev->major, dev->minor, FSCK_MSDOS_PATH);
        return 0;
    }

#ifdef VERIFY_PASS
    char *args[7];
    args[0] = FSCK_MSDOS_PATH;
    args[1] = "-v";
    args[2] = "-V";
    args[3] = "-w";
    args[4] = "-p";
    args[5] = blkdev_get_devpath(dev);
    args[6] = NULL;
    rc = logwrap(6, args);
    free(args[5]);
#else
    char *args[6];
    args[0] = FSCK_MSDOS_PATH;
    args[1] = "-v";
    args[2] = "-w";
    args[3] = "-p";
    args[4] = blkdev_get_devpath(dev);
    args[5] = NULL;
    rc = logwrap(5, args);
    free(args[4]);
#endif

    if (rc == 0) {
        LOG_VOL("Filesystem check completed OK");
        return 0;
    } else if (rc == 1) {
        LOG_VOL("Filesystem check failed (general failure)");
        return -EINVAL;
    } else if (rc == 2) {
        LOG_VOL("Filesystem check failed (invalid usage)");
        return -EIO;
    } else if (rc == 4) {
        LOG_VOL("Filesystem check completed (errors fixed)");
    } else if (rc == 8) {
        LOG_VOL("Filesystem check failed (not a FAT filesystem)");
        return -ENODATA;
    } else {
        LOG_VOL("Filesystem check failed (unknown exit code %d)", rc);
        return -EIO;
    }
    return 0;
}

int vfat_mount(blkdev_t *dev, volume_t *vol, boolean safe_mode)
{
    int flags, rc;
    char *devpath;

    devpath = blkdev_get_devpath(dev);

#if VFAT_DEBUG
    LOG_VOL("vfat_mount(%d:%d, %s, %d):", dev->major, dev->minor, vol->mount_point, safe_mode);
#endif

    flags = MS_NODEV | MS_NOEXEC | MS_NOSUID | MS_DIRSYNC;

    if (vol->state == volstate_mounted) {
        LOG_VOL("Remounting %d:%d on %s, safe mode %d", dev->major,
                dev->minor, vol->mount_point, safe_mode);
        flags |= MS_REMOUNT;
    }

    rc = mount(devpath, vol->mount_point, "vfat", flags,
               "utf8,uid=1000,gid=1000,fmask=711,dmask=700,shortname=mixed");

    if (rc && errno == EROFS) {
        LOGE("vfat_mount(%d:%d, %s): Read only filesystem - retrying mount RO",
             dev->major, dev->minor, vol->mount_point);
        flags |= MS_RDONLY;
        rc = mount(devpath, vol->mount_point, "vfat", flags,
                   "utf8,uid=1000,gid=1000,fmask=711,dmask=700,shortname=mixed");
    }

#if VFAT_DEBUG
    LOG_VOL("vfat_mount(%s, %d:%d): mount rc = %d",
             vol->mount_point,dev->major, dev->minor, rc);
#endif
    free (devpath);
    return rc;
}
