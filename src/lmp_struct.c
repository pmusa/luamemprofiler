/*
**
** Author: Pablo Musa
** Creation Date: aug 16 2011
** Last Modification: aug 22 2011
** See Copyright Notice in COPYRIGHT
**
** See lmp_struct.h for module overview
**
*/


#include <stdint.h>
#include "lmp_struct.h"
#include "lua.h"

#define HASH_SIZE 23  /* empiric hash size - need more tests to confirm */


/* simple hash function */
static int hashfunc(void *ptr) {
  return ((uintptr_t) ptr) % HASH_SIZE;
}


/* STATIC GLOBAL VARIABLE */
static lmp_Block **lmp_head = NULL; /* hashtable for all blocks */
static int usegraphics;


/* GLOBAL VARIABLES - filter lists */
/* multiply linked lists used for type filtering. used only in graphic mode */
lmp_Block *lmp_string = NULL;
lmp_Block *lmp_function = NULL;
lmp_Block *lmp_userdata = NULL;
lmp_Block *lmp_thread = NULL;
lmp_Block *lmp_table = NULL;
lmp_Block *lmp_other = NULL;
lmp_Block *lmp_all = NULL; /* used to redraw all blocks */


void st_newhash(int usegraphic) {
  int i;
  usegraphics = usegraphic;
  lmp_head = (lmp_Block **) malloc (HASH_SIZE * sizeof(lmp_Block*));
  for (i = 0; i < HASH_SIZE; i++) {
    lmp_head[i] = NULL;
  }
}

void st_destroyhash() {
  int i;

  for(i = 0; i < HASH_SIZE; i++) {
    lmp_Block *p;
    lmp_Block *head = lmp_head[i];
    while (head != NULL) {
      p = head;
      head = head->next;
      free(p);
    }
  }
  free(lmp_head);

  if (usegraphics) {
    lmp_string = NULL;
    lmp_function = NULL;
    lmp_userdata = NULL;
    lmp_thread = NULL;
    lmp_table = NULL;
    lmp_other = NULL;
    lmp_all = NULL;
  }
}

lmp_Block *st_removeblock (void *ptr) {
  lmp_Block *p, *ant = NULL;
  int i = hashfunc(ptr);
  for (p = lmp_head[i]; p != NULL; ant = p, p = p->next) {
    if (p->ptr == ptr) {
      if (ant == NULL) {
        lmp_head[i] = p->next;
      } else {
        ant->next = p->next;
      }
      p->next = NULL;

      if (usegraphics) {
        if (p->prevtype != NULL) {
          p->prevtype->nexttype = p->nexttype;
        }
        if (p->nexttype != NULL) {
          p->nexttype->prevtype = p->prevtype;
        }

        if (p->prevall != NULL) {
          p->prevall->nextall = p->nextall;
        }
        if (p->nextall != NULL) {
          p->nextall->prevall = p->prevall;
        }
      }

      return p;
    }
  }
  return NULL;
}

void st_insertblock (lmp_Block *block) {
  lmp_Block **type;

  int i = hashfunc(block->ptr);
  block->next = lmp_head[i];
  lmp_head[i] = block;

  if (usegraphics) {
    switch (block->luatype) {
      case LUA_TSTRING:
        type = &lmp_string;
        break;
      case LUA_TFUNCTION:
        type = &lmp_function;
        break;
      case LUA_TUSERDATA:
        type = &lmp_userdata;
        break;
      case LUA_TTHREAD:
        type = &lmp_thread;
        break;
      case LUA_TTABLE:
        type = &lmp_table;
        break;
      default:  /* OTHER */
        type = &lmp_other;
        break;
    }
    if (*type != NULL) {
      (*type)->prevtype = block;
    }
    block->nexttype = *type;
    block->prevtype = NULL;
    *type = block;

    if (lmp_all != NULL) {
      lmp_all->prevall = block;
    }
    block->nextall = lmp_all;
    block->prevall = NULL;
    lmp_all = block;
  }
}

void st_initblock (lmp_Block *block, void *ptr, size_t size, size_t luatype) {
  block->ptr = ptr;
  block->size = size;
  block->luatype = luatype;
  block->next = NULL;
  if (usegraphics) {
    block->nexttype = NULL;
    block->prevtype = NULL;
    block->nextall = NULL;
    block->prevall = NULL;
  }
}

void *st_getptr(lmp_Block *block) {
  return block->ptr;
}

size_t st_getsize(lmp_Block *block) {
  return block->size;
}

size_t st_getluatype(lmp_Block *block) {
  return block->luatype;
}

lmp_Block *st_getnext(lmp_Block *block) {
  return block->next;
}

lmp_Block *st_getnexttype(lmp_Block *block) {
  return usegraphics ? block->nexttype : NULL;
}

lmp_Block *st_getprevtype(lmp_Block *block) {
  return usegraphics ? block->prevtype : NULL;
}

lmp_Block *st_getnextall(lmp_Block *block) {
  return usegraphics ? block->nextall : NULL;
}

lmp_Block *st_getprevall(lmp_Block *block) {
  return usegraphics ? block->prevall : NULL;
}

void st_setsize(lmp_Block *block, size_t size) {
  block->size = size;
}

void st_setptr(lmp_Block *block, void *ptr) {
  block->ptr = ptr;
}

