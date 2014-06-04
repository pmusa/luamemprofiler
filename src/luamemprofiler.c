/*
** Author: Pablo Musa
** Creation Date: mar 27 2011
** Last Modification: aug 22 2011
** See Copyright Notice in COPYRIGHT
**
** This module is responsible for registering the luamemprofiler lib in the
** Lua environment. It also sets a finalizer for the luamemprofiler library
** which restores the lua_State original function when the library is garbage
** collected.
** The library implements two main functions (start and stop).
** The start function receives an optional parameter (a number containing
** the expected memory consumption) which determines if the library will
** display real-time information and the granularity of the blocks.
**
 */

#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdint.h>

#include "lmp.h"

/* Keeps the default allocation function and the ud of a lua_State */
typedef struct lmp_allocstructure {
  lua_Alloc f;
  void *ud;
} lmp_Alloc;

/*
** Called when main program ends.
** Restores lua_State original allocation function.
*/
static int finalize (lua_State *L) {
  lmp_Alloc *s;

  /* check lmp_Alloc */
  if (!lua_isuserdata(L, -1)) {
    lua_pushstring(L, "incorrect argument");
    lua_error(L);
  }

  /* get lmp_Alloc and restore original allocation function */
  s = (lmp_Alloc *) lua_touserdata(L, -1);
  if (s->f != lua_getallocf (L, NULL)) {
    lua_setallocf(L, s->f, s->ud);
    lmp_stop();
  }

  return 0;
}

/* Register finalize function as metatable */
static void create_finalizer(lua_State *L, lua_Alloc f, void *ud) {
  lmp_Alloc *s;

  /* create metatable with finalize function (__gc field) */
  luaL_newmetatable(L, "luamemprofiler_mt");
  lua_pushcfunction(L, finalize);
  lua_setfield(L, -2, "__gc");

  /* create 'alloc' userdata (one ud for each Lua_State) */
  s = (lmp_Alloc*) lua_newuserdata(L, (size_t) sizeof(lmp_Alloc));
  s->f = f;
  s->ud = ud;

  /* set userdata metatable */
  luaL_setmetatable(L, "luamemprofiler_mt");

  /* insert userdata into registry table so it cannot be collected */
  lua_setfield(L, LUA_REGISTRYINDEX, "luamemprofiler_ud");
}

/* Main module function. Starts the library */
static int luamemprofiler_start(lua_State *L) {
  static lua_Alloc f;
  static void *ud;

  float memused;
  int usegraphics = 0;

  /* get the amount of memory expected to be used AND set enable graphics */
  memused = (float) lua_tonumber(L, 1);
  if (memused)
    usegraphics = 1;

  /* get default allocation function */
  f = lua_getallocf(L, &ud);

  /* check if start has been called before */
  if (f == lmp_alloc) {
    /* restore default allocation function and remove library finalizer */
    lmp_Alloc *s;
    lua_getfield(L, LUA_REGISTRYINDEX, "luamemprofiler_ud");
    s = (lmp_Alloc *) lua_touserdata(L, -1);
    lua_setallocf(L, s->f, s->ud);
    lua_getmetatable(L, -1);
    lua_pushnil(L);
    lua_setfield(L, -2, "__gc");

    lua_pushstring(L, "calling luamemprofiler start function twice");
    lua_error(L);
  }

  /* create data_structure and set finalizer */
  create_finalizer(L, f, ud);
  lua_setallocf(L, lmp_alloc, ud);

  /* L is in most cases the lowest address of the heap (easiest to access) */
  lmp_start((uintptr_t) L, memused, usegraphics);
  return 0;
}

/* restore default allocation function and stop the other modules */
static int luamemprofiler_stop(lua_State *L) {
  lmp_Alloc *s;

  /* get 'alloc' userdata and restore original allocation function */
  lua_pushstring(L, "luamemprofiler_ud");
  lua_rawget(L, LUA_REGISTRYINDEX);
  s = (lmp_Alloc*) lua_touserdata(L, -1);
  if (s == NULL) {
    lua_pushstring(L, "calling luamemprofiler stop function without calling start function");
    lua_error(L);
  }
  lua_pop(L, 1);
  lua_setallocf(L, s->f, s->ud);

  lmp_stop();
  return 0;
}


/**********************************
 * register structs and functions *
 **********************************/

/* luamemprofiler function registration array */
static const luaL_Reg luamemprofiler[] = {
  { "start", luamemprofiler_start},
  { "stop", luamemprofiler_stop},
  { NULL, NULL }
};

/* register luamemprofiler functions */
LUALIB_API int luaopen_luamemprofiler (lua_State *L) {
  luaL_newlib(L, luamemprofiler);
  return 1;
}
