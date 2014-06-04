/*
**
** Author: Pablo Musa
** Creation Date: aug 16 2011
** Last Modification: aug 22 2011
** See Copyright Notice in COPYRIGHT
**
** This module is responsible by defining the block structure used keep
** information of each allocation and by implementing data structures to
** hold these blocks. The main data structure is a hash table with predefined
** size and separate chaining with list heads. There are other seven multiply
** linked lists used for type filtering. However these lists are used just in
** the graphic module and do not produce overhead when graphics are disabled.
** 
*/


#ifndef LMP_LMPSTRUCT_H
#define LMP_LMPSTRUCT_H


#include <stdlib.h>


/*
** Holds memory address, size and type of each block allocated.
** Can have connection with 3 structures (hash table, type list and all list).
** The hash table is the module main structure, the 'type list' is the list
** where all blocks of a specific types are linked. The 'all list' is a list
** where all blocks are sequentially linked.
*/
struct lmp_block {
  void *ptr;
  size_t size;
  size_t luatype;
  struct lmp_block *next;
  struct lmp_block *nexttype;
  struct lmp_block *prevtype;
  struct lmp_block *nextall;
  struct lmp_block *prevall;
};
typedef struct lmp_block lmp_Block;


/*
** Sets global usegraphics, malloc and initialize the hash table.
*/
void st_newhash(int usegraphic);

/*
** Destroy and free the hash table and if usegraphics reset filter lists.
*/
void st_destroyhash();

/*
** Searches for a block with specified ptr address. If the block is found,
** removes the block from the hash table. If usegraghics, also removes
** the block from his specific 'filter list' and from 'all list'.
*/
lmp_Block *st_removeblock (void *ptr);

/*
** Inserts the specified block into the hash table. If usegraphics, also
** inserts the block into his specific 'filter list' and into 'all list'.
*/
void st_insertblock (lmp_Block *block);

/*
** Initializes the specified block with the specified values and sets the
** hash table pointer to NULL. If usegraphics, initialize the other pointers.
*/
void st_initblock (lmp_Block *block, void *ptr, size_t nsize, size_t luatype);

/*
** Gets and Sets.
*/
void *st_getptr(lmp_Block *block);
size_t st_getsize(lmp_Block *block);
size_t st_getluatype(lmp_Block *block);
lmp_Block *st_getnext(lmp_Block *block);
lmp_Block *st_getnexttype(lmp_Block *block);
lmp_Block *st_getprevtype(lmp_Block *block);
lmp_Block *st_getnextall(lmp_Block *block);
lmp_Block *st_getprevall(lmp_Block *block);

void st_setsize(lmp_Block *block, size_t size);
void st_setptr(lmp_Block *block, void *ptr);

#endif
