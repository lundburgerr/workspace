/* --COPYRIGHT--,BSD
 * Copyright (c) $(CPYYEAR), Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/* 
 *  The SysStd System provider is a good one to use for debugging 
 *  but does not have the best performance. Use xdc.runtime.SysMin
 *  for better performance.
 */
var System   = xdc.useModule('xdc.runtime.System');
var SysStd   = xdc.useModule('xdc.runtime.SysStd');
var Mailbox = xdc.useModule('ti.sysbios.knl.Mailbox');
var Memory = xdc.useModule('xdc.runtime.Memory');
System.SupportProxy = SysStd;

/* Task that does the notify sending */
/*
var Task = xdc.useModule('ti.sysbios.knl.Task');
var tsk0 = Task.create('&tsk0_func');
tsk0.instance.name = "tsk0";
*/

/* 
 * The BIOS module will create the default heap for the system.
 * Specify the size of this default heap.
 */





/* Modules explicitly used in the application */
var MessageQ                = xdc.useModule('ti.sdo.ipc.MessageQ');  
var Ipc                     = xdc.useModule('ti.sdo.ipc.Ipc');
var HeapBufMP               = xdc.useModule('ti.sdo.ipc.heaps.HeapBufMP');
//var BIOS                    = xdc.useModule('ti.sysbios.BIOS');

/* Synchronize all processors (this will be done in Ipc_start) */
Ipc.procSync = Ipc.ProcSync_NONE;

/* Shared Memory Map */
var SHAREDMEM           = 0xC3000000;
var SHAREDMEMSIZE       = 0x00020000;

/* 
 *  Need to define the shared region. The IPC modules use this
 *  to make portable pointers. All processors need to add this
 *  call with their base address of the shared memory region.
 *  If the processor cannot access the memory, do not add it.
 */ 
var SharedRegion = xdc.useModule('ti.sdo.ipc.SharedRegion');
SharedRegion.setEntryMeta(0,
    { base: SHAREDMEM, 
      len:  SHAREDMEMSIZE,
      ownerProcId: 0,
      isValid: true,
      name: "shared_mem",
    });
    
// BIOS.heapSize = 0x16000;

/* System stack size (used by ISRs and Swis) */
Program.stack = 0x8000;
Program.heap = 4000;
//tsk0.priority = 10;
