/*
 * Copyright (C) 2012 The Android Open Source Project
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

#include <stdbool.h>

#define LOG_TAG "libsuspend"
#include <cutils/log.h>

#include <suspend/autosuspend.h>

#include "autosuspend_ops.h"

enum {
  STATE_UNINIT,
  STATE_FAILED,
  STATE_DISABLED,
  STATE_ENABLED
};

static struct autosuspend_ops *autosuspend_ops;
static int autosuspend_state = STATE_UNINIT;

static int autosuspend_init(void)
{
    if (autosuspend_state != STATE_UNINIT) {
        return autosuspend_state;
    }

    autosuspend_ops = autosuspend_earlysuspend_init();
    if (autosuspend_ops) {
        goto out;
    }

    autosuspend_ops = autosuspend_autosleep_init();
    if (autosuspend_ops) {
        goto out;
    }

    autosuspend_ops = autosuspend_wakeup_count_init();
    if (autosuspend_ops) {
        goto out;
    }

    if (!autosuspend_ops) {
        ALOGE("failed to initialize autosuspend\n");
        autosuspend_state = STATE_FAILED;
        return autosuspend_state;
    }

out:
    autosuspend_state = STATE_DISABLED;

    ALOGV("autosuspend initialized\n");
    return autosuspend_state;
}

int autosuspend_enable(void)
{
    int ret;

    if (autosuspend_init() == STATE_FAILED) {
        return -1;
    }

    ALOGV("autosuspend_enable\n");

    if (autosuspend_state == STATE_ENABLED) {
        return 0;
    }

    ret = autosuspend_ops->enable();
    if (ret) {
        return ret;
    }

    autosuspend_state = STATE_ENABLED;
    return 0;
}

int autosuspend_disable(void)
{
    int ret;

    if (autosuspend_init() == STATE_FAILED) {
        return -1;
    }

    ALOGV("autosuspend_disable\n");

    if (autosuspend_state == STATE_DISABLED) {
        return 0;
    }

    ret = autosuspend_ops->disable();
    if (ret) {
        return ret;
    }

    autosuspend_state = STATE_DISABLED;
    return 0;
}
