/*
**
** Author: Pablo Musa
** Creation Date: may 27 2011
** Last Modification: aug 22 2011
** See Copyright Notice in COPYRIGHT
** 
** See lmp.h for module overview
**
*/

#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "lmp.h"
#include "vmemory.h"
#include "lmp_struct.h"

#define LMP_FREE 0
#define LMP_MALLOC 1
#define LMP_REALLOC 2


/* STATIC VARIABLES */
/* ac = allocation counter */
static int ac_string, ac_function, ac_userdata, ac_thread, ac_table, ac_other;

static long nallocs, alloc_size;
static long nreallocs, realloc_size;
static long nfrees, free_size;
static long memoryuse, maxmemoryuse;
static int Laddress;
static int Maddress = 0;
static int usegraphics;

/* STATIC FUNCTIONS */
static void *lmp_malloc(size_t nsize, size_t osize);
static void *lmp_free(void *ptr);
static void *lmp_realloc(void *ptr, size_t nsize);
static void initcounters();
static void updatecounters(int alloctype, size_t size, size_t luatype);
static void generatereport();

/* PUBLIC FUNCTIONS */
void lmp_start(int lowestaddress, float memused, int usegraphic) {
  initcounters();
  st_newhash(usegraphic);
  usegraphics = usegraphic;
  Laddress = lowestaddress;  /* save lowest address to calc mem needed */
  if (usegraphics)
    vm_start(lowestaddress, memused);
}

void lmp_stop() {
  generatereport();

  /* erase counters and blocks */
  initcounters();
  st_destroyhash();
  if (usegraphics)
    vm_stop();
}

/* allocation function used by Lua when luamemprofiler is used */
void *lmp_alloc (void *ud, void *ptr, size_t osize, size_t nsize) {
  (void) ud;
  (void) osize;

  if (nsize == 0) {  /* calls our malloc, free or realloc functions */
    return lmp_free(ptr);
  } else if (ptr == NULL) {
    return lmp_malloc(nsize, osize);  /* osize is the lua_type */
  } else { 
    return lmp_realloc(ptr, nsize);
  }
}

/* STATIC FUNCTIONS */

/* does normal malloc and then alloc and update other structures */
static void *lmp_malloc(size_t nsize, size_t luatype) {
  void *ptr = malloc(nsize); /* normal malloc */
  lmp_Block *new = (lmp_Block *) malloc (sizeof(lmp_Block));

  st_initblock(new, ptr, nsize, luatype);
  st_insertblock(new);

  updatecounters(LMP_MALLOC, nsize, luatype);
  if ((int) ptr > Maddress)  /* save max address to calc mem needed */
    Maddress = (int) ptr;

  if (usegraphics)  /* if graphics enabled call function to handle */
    vm_newmemop(LMP_VM_MALLOC, ptr, luatype, nsize);

  return ptr;
}

/* free and update other structures and then does normal free */
static void *lmp_free(void *ptr) {
  lmp_Block *block = st_removeblock(ptr);
  if (block != NULL) {
    int size = st_getsize(block);
    updatecounters(LMP_FREE, size, 0);
    if (usegraphics) {  /* if graphics enabled call function to handle */
      vm_newmemop(LMP_VM_FREE, ptr, LUA_TFREE, size);
    }
    free(block);
  }
  free(ptr);
  return NULL;
}

/* 
** does normal realloc, assumes realloc always change object address (wich is
** not true, but is simplier to program and costless) and updates block
** information. then, if usegraphics, verify if realloc is enlarging or
** shrinking and call vm_newop with the correct values. Optimise drawing if
** realloc uses same object address. Finally, update counters.
*/

static void *lmp_realloc(void *ptr, size_t nsize) {
  lmp_Block *block;
  size_t osize;
  void *p = realloc(ptr, nsize);
  if (p == NULL) return NULL;

  block = st_removeblock(ptr);  /* realloc usually changes memory address */
  if (block != NULL) {
    osize = st_getsize(block);
    st_setsize(block, nsize);
    st_setptr(block, p);
    st_insertblock(block);
    if (usegraphics) {
      int luatype = st_getluatype(block);
      if (ptr != p) {  /* memory location changed */
        vm_newmemop(LMP_VM_REALLOC, ptr, LUA_TFREE, osize); /*erase old block*/
        vm_newmemop(LMP_VM_REALLOC, p, luatype, nsize);
      } else {
        if (nsize > osize) {  /* enlarging block */
          vm_newmemop(LMP_VM_REALLOC, (char *) ptr + osize, luatype, nsize - osize);
        } else if (osize > nsize) {  /* shrinking block - erase extra part */
          vm_newmemop(LMP_VM_REALLOC, (char*) ptr + nsize, LUA_TFREE, osize - nsize);
        }
      }
    }
    updatecounters(LMP_REALLOC, nsize - osize, 0);
  }
  return p;
}

/* STATIC FUNCTIONS */
static void initcounters() {
  ac_string=0;ac_function=0;ac_userdata=0;ac_thread=0;ac_table=0;ac_other=0;
  nallocs=0;alloc_size=0;
  nreallocs=0;realloc_size=0;
  nfrees=0;free_size=0;
  memoryuse=0;maxmemoryuse=0;
}

/* check alloctype and update counters accordingly */
static void updatecounters (int alloctype, size_t size, size_t luatype) {
  if (alloctype == LMP_FREE) {
    nfrees = nfrees + 1;
    free_size = free_size + size;
    memoryuse = memoryuse - size;
  } else if (alloctype == LMP_REALLOC) {
    nreallocs = nreallocs + 1;
    realloc_size = realloc_size + size;
    memoryuse = memoryuse + size;
    if (memoryuse > maxmemoryuse) {
      maxmemoryuse = memoryuse;
    }
  } else if (alloctype == LMP_MALLOC) {
    nallocs = nallocs + 1;
    alloc_size = alloc_size + size;
    memoryuse = memoryuse + size;
    if (memoryuse > maxmemoryuse) {
      maxmemoryuse = memoryuse;
    }
    switch(luatype) {
      case LUA_TSTRING:
        ac_string++;
        break;
      case LUA_TFUNCTION:
        ac_function++;
        break;
      case LUA_TUSERDATA:
        ac_userdata++;
        break;
      case LUA_TTHREAD:
        ac_thread++;
        break;
      case LUA_TTABLE:
        ac_table++;
        break;
      default:
        ac_other++;
    }
  }
}

/* 
** writes the report in the standard output. If not usegraphics, calculates
** program memory usage and sugest memory consumption parameter for future
** execution.
 */
static void generatereport() {
  float mem = ((float) (Maddress - Laddress) / 1000000) + 0.1;

  if (!usegraphics)
    mem = mem + 0.4;  /* empiric size of graphic mem usage */

printf("===================================================================\n");
printf("Number of Mallocs=%ld\tTotal Malloc Size=%ld\n", nallocs, alloc_size);
printf("Number of Reallocs=%ld\tTotal Realloc Size=%ld\n", nreallocs, realloc_size);
printf("Number of Frees=%ld\tTotal Free Size=%ld\n", nfrees, free_size);
printf("\nNumber of Allocs of Each Type:\n");
printf("  String=%d | Function=%d | Usedata=%d | Thread=%d | Table=%d | Other=%d\n", ac_string, ac_function, ac_userdata, ac_thread, ac_table, ac_other);
printf("\nMaximum Memory Used=%ld bytes\n", maxmemoryuse);

  if (!usegraphics && nallocs > 0) {
printf("\nWe suggest you run the application again using %.1f as parameter\n", mem); 
  }
printf("===================================================================\n");
}

