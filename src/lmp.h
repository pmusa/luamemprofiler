/*
**
** Author: Pablo Musa
** Creation Date: may 27 2011
** Last Modification: aug 22 2011
** See Copyright Notice in COPYRIGHT
** 
** This module is responsible by defining the new allocation function and
** by collecting all information about memory management.
** At the end of the program execution or when luamemprofiler.stop() is
** called, it generates a log containing several memory information.
**
*/

#ifndef LMP_LMP_H
#define LMP_LMP_H

/*
** Initializes the counters, sets the lowest address of the heap and
** enables/disables the use of the graphic module (vm_start).
*/
void lmp_start (int lowestaddress, float memused, int usegraphics);

/*
** Finalizes the counters, free all blocks structures, stop the graphic
** module (vm_stop) [if started] and generates the report (number of: mallocs,
** frees, tables, ...).
*/
void lmp_stop ();

/*
** Checks the alloc type (malloc, free, realloc) and update data in
** accordance. Create, remove or update block structures, update report
** counters (mallocs, tables, etc.) and call vm_newmemop if graphic
** module is enabled.
*/
void *lmp_alloc (void *ud, void *ptr, size_t osize, size_t nsize);

#endif
