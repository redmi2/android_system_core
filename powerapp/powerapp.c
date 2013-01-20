/*
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions are
 *   met:
 *     * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/input.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <errno.h>
#include <linux/reboot.h>
#include <libgen.h>

#define KEY_INPUT_DEVICE "/dev/input/event0"
#define SHUTDOWN_COMMAND "/sbin/shutdown"
#define REBOOT_COMMAND "/sbin/reboot"
#define USEC_IN_SEC 1000000
#define POWER_NODE "/sys/power/state"
#define BUFFER_SZ 32
#define SUSPEND_STRING "mem"
#define RESUME_STRING "on"
#define POWER_OFF_TIMER 1000000

int diff_timestamps(struct timeval* then, struct timeval* now)
{
   if (now->tv_usec > then->tv_usec)
   {
      now->tv_usec += USEC_IN_SEC;
      now->tv_sec--;
   }
   return (int) (now->tv_sec - then->tv_sec)*USEC_IN_SEC + now->tv_usec - then->tv_usec;
}

void powerapp_shutdown(void)
{
   pid_t pid;
   
   printf("SHUTDOWN\n");
   return;
   pid = fork();
   if (pid == 0)
   {
      execl(SHUTDOWN_COMMAND, SHUTDOWN_COMMAND, NULL);
   }
   // should never reach here
   for(;;);
}

void sys_shutdown_or_reboot(int reboot, char *arg1)
{
   int cmd = LINUX_REBOOT_CMD_POWER_OFF;
   int n = 0;

   if (reboot)
   {
      if (arg1)
          cmd = LINUX_REBOOT_CMD_RESTART2;
      else
          cmd = LINUX_REBOOT_CMD_RESTART;
   }

   n = syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, cmd, arg1);
   if (n < 0)
   {
      fprintf(stderr, "reboot system call failed %d (%s)\n", errno, strerror(errno));
   }
}

void suspend_or_resume(void)
{
   int fd = -1;
   char buf[BUFFER_SZ];
   int n = 0;
   static int suspend = 1;

   printf("Power Key Initiated System Suspend or Resume\n");

   fd = open(POWER_NODE, O_WRONLY);

   if (fd > 0)
   {
     if (suspend == 1)
     {
       strcpy (buf, SUSPEND_STRING);
       errno = 0;
       if (write(fd, buf, strlen(buf)) == -1)
       {
           printf("Suspend failed %d (%s)\n", errno, strerror(errno));
       }
       suspend = 0;
      }
      else
      {
        suspend = 1;
      }
    }

    close(fd);
}


int
main(int argc, char *argv[])
{
   int fd = 0;
   struct input_event ev;
   struct timeval then;
   struct timeval now;
   int n = 0;
   int duration = 0;
   char *arg1 = NULL;
   char *cmd_name = basename(argv[0]);
   if(argc > 1)
	   arg1 = argv[1];

   if (!strcmp(cmd_name, "sys_reboot"))
   {
      sys_shutdown_or_reboot(1, arg1);
      return 1;
   }
   else if (!strcmp(cmd_name, "sys_shutdown"))
   {
      sys_shutdown_or_reboot(0, arg1);
      return 2;
   }
   fd = open(KEY_INPUT_DEVICE, O_RDONLY);
   if (fd == -1)
   {
      fprintf(stderr, "%s: cannot open input device %s\n", argv[0], KEY_INPUT_DEVICE);
      exit(1);
   }

   memset(&then, 0, sizeof(struct timeval));
   memset(&now, 0, sizeof(struct timeval));

   while ((n = read(fd, &ev, sizeof(struct input_event))) > 0) {
      if (n < sizeof(struct input_event))
      {
	 fprintf(stderr, "%s: cannot read whole input event\n", argv[0]);
	 exit(2);
      }

      if (ev.type == EV_KEY && ev.code == KEY_POWER && ev.value == 1)
      {
	 memcpy(&then, &ev.time, sizeof(struct timeval));
      }
      else if (ev.type == EV_KEY && ev.code == KEY_POWER && ev.value == 0)
      {
	 memcpy(&now, &ev.time, sizeof(struct timeval));
	 duration = diff_timestamps(&then, &now);
	 if (duration > POWER_OFF_TIMER)
	 {
	    powerapp_shutdown();
	 }
	 else
	 {
	    suspend_or_resume();
	 }
      }

/*      printf("%d.%06d: type=%d code=%d value=%d\n",
	     (int) ev.time.tv_sec, (int) ev.time.tv_usec,
	     (int) ev.type, ev.code, (int) ev.value); */
   }

   return 0;
}
