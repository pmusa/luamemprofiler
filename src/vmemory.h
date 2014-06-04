/*
**
** Author: Pablo Musa
** Creation Date: mar 27 2011
** Last Modification: aug 22 2011
** See Copyright Notice in COPYRIGHT
**
** This module manages the graphical display offered by the library. It defines
** the size of each element: right column, bottom row, memory box, box border,
** etc. It also dynamically calculates: block's bytes per pixel based on the
** memory consumption; the relative address of each pointer; the x, y
** coordinate to the block; the zoom in and zoom out.
** Finally, this module is responsible by managing the events and the
** corresponding actions.
**
*/

#ifndef LMP_VMEMORY_H
#define LMP_VMEMORY_H

#include "graphic.h"

#define LMP_VM_FREE    0
#define LMP_VM_MALLOC  1
#define LMP_VM_REALLOC 2
#define LUA_TFREE 11

#define LMP_VM_STRING_CL     DARKRED
#define LMP_VM_FUNCTION_CL   DARKMAGENTA
#define LMP_VM_USERDATA_CL   DARKGREEN
#define LMP_VM_THREAD_CL     DARKORANGE
#define LMP_VM_TABLE_CL      DARKBLUE
#define LMP_VM_OTHER_CL      DIMGRAY
#define LMP_VM_FREE_CL       WHITE

#define LMP_VM_BACKGROUND_CL LTGRAY
#define LMP_VM_MEMBOX_CL     WHITE


/*
** Calculates window size (based on expected memory consumption).
** Initializes the whole window (memory box, bottom row, right column).
*/
void vm_start(int lowestaddress, float memused);

/*
** Calls gr_destroyscreen to destroy the window.
*/
void vm_stop();

/*
** Module main function. It is responsible by managing new memory operations
** and user events.
*/
void vm_newmemop(int memop, void *ptr, size_t luatype, size_t size);

#endif
