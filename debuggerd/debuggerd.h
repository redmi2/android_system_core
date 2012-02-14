/* system/debuggerd/debuggerd.h
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <cutils/logd.h>
#include <sys/ptrace.h>
#include <unwind.h>
#include "utility.h"
#include "symbol_table.h"


/* Main entry point to get the backtrace from the crashing process */
extern int unwind_backtrace_with_ptrace(int tfd, pid_t pid, mapinfo *map,
                                        unsigned int sp_list[],
                                        int *frame0_pc_sane,
                                        bool at_fault);

extern void dump_registers(int tfd, int pid, bool at_fault);

extern int unwind_backtrace_with_ptrace_x86(int tfd, pid_t pid, mapinfo *map, bool at_fault);

void dump_pc_and_lr(int tfd, int pid, mapinfo *map, int unwound_level, bool at_fault);

void dump_stack_and_code(int tfd, int pid, mapinfo *map,
                         int unwind_depth, unsigned int sp_list[],
                         bool at_fault);

/* dalvik constatnts */
/* copied from /dalvik/vm/mterp/common/asm-constants.h */
# define MTERP_OFFSET(name, type, field, offset)    int name = offset;
# define MTERP_SIZEOF(name, type, size)             int name = size;
# define MTERP_CONSTANT(name, value)                int name = value;

/* struct Thread */
MTERP_OFFSET(offThread_pc,                Thread, interpSave.pc, 0)
MTERP_OFFSET(offThread_curFrame,          Thread, interpSave.curFrame, 4)
MTERP_OFFSET(offThread_method,            Thread, interpSave.method, 8)
MTERP_OFFSET(offThread_methodClassDex,    Thread, interpSave.methodClassDex, 12)
MTERP_OFFSET(offThread_threadId,          Thread, threadId, 36)
MTERP_OFFSET(offThread_inJitCodeCache,    Thread, inJitCodeCache, 124)

/* struct Method */
MTERP_OFFSET(offMethod_clazz,           Method, clazz, 0)
MTERP_OFFSET(offMethod_accessFlags,     Method, accessFlags, 4)
MTERP_OFFSET(offMethod_methodIndex,     Method, methodIndex, 8)
MTERP_OFFSET(offMethod_registersSize,   Method, registersSize, 10)
MTERP_OFFSET(offMethod_outsSize,        Method, outsSize, 12)
MTERP_OFFSET(offMethod_name,            Method, name, 16)
MTERP_OFFSET(offMethod_shorty,          Method, shorty, 28)
MTERP_OFFSET(offMethod_insns,           Method, insns, 32)
MTERP_OFFSET(offMethod_nativeFunc,      Method, nativeFunc, 40)

/* ClassObject fields */
MTERP_OFFSET(offClassObject_descriptor, ClassObject, descriptor, 24)
MTERP_OFFSET(offClassObject_accessFlags, ClassObject, accessFlags, 32)
MTERP_OFFSET(offClassObject_pDvmDex,    ClassObject, pDvmDex, 40)
MTERP_OFFSET(offClassObject_status,     ClassObject, status, 44)
MTERP_OFFSET(offClassObject_super,      ClassObject, super, 72)
MTERP_OFFSET(offClassObject_vtableCount, ClassObject, vtableCount, 112)
MTERP_OFFSET(offClassObject_vtable,     ClassObject, vtable, 116)
