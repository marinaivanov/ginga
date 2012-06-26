/* nclua-private.h -- Exported private declarations.
   Copyright (C) 2006-2012 PUC-Rio/Laboratorio TeleMidia

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef NCLUA_PRIVATE_H
#define NCLUA_PRIVATE_H

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "nclua.h"
#include "nclua-compiler-private.h"

NCLUA_BEGIN_DECLS

#include <lua.h>

/* The Lua structures used by the NCLua engine are kept in an internal table
   called the "NCLua Store", or "store" for short.  This table is stored in
   the Lua registry, indexed by the address of the following global.  */
NCLUA_PRIVATE const int _nclua_magic;

/* Keys for data in store table.  */
enum
{
  _NCLUA_STORE_EMPTY_TABLE_KEY, /* key of an empty table */
  _NCLUA_STORE_STATE_KEY,       /* key of NCLua state */
  _NCLUA_STORE_USER_DATA_KEY,   /* key of table with attached user data */
  _NCLUA_STORE_EVENT_KEY,       /* key of the NCLua Event store table */
  _NCLUA_STORE_LAST_KEY,        /* total number of key values */
};

/* Sets the value on top of stack as the new store table.
   This macro pops the value from stack.  */
#define _nclua_set_store(L)                             \
  NCLUA_STMT_BEGIN                                      \
  {                                                     \
    lua_pushvalue (L, LUA_REGISTRYINDEX);               \
    lua_pushlightuserdata (L, (void *) &_nclua_magic);  \
    lua_pushvalue (L, -3);                              \
    lua_rawset (L, -3);                                 \
    lua_pop (L, 2);                                     \
  }                                                     \
  NCLUA_STMT_END

/* Creates a new store table and inserts it into Lua registry.  */
#define _nclua_create_and_set_store(L)                  \
  NCLUA_STMT_BEGIN                                      \
  {                                                     \
    lua_createtable (L, _NCLUA_STORE_LAST_KEY, 0);      \
    _nclua_set_store (L);                               \
  }                                                     \
  NCLUA_STMT_END

/* Pushes onto stack the store table.  */
#define _nclua_get_store(L)                             \
  NCLUA_STMT_BEGIN                                      \
  {                                                     \
    lua_pushvalue (L, LUA_REGISTRYINDEX);               \
    lua_pushlightuserdata (L, (void *) &_nclua_magic);  \
    lua_rawget (L, -2);                                 \
    lua_remove (L, -2);                                 \
  }                                                     \
  NCLUA_STMT_END

/* Pushes onto stack the value store[KEY] */
#define _nclua_get_store_data(L, key)           \
  NCLUA_STMT_BEGIN                              \
  {                                             \
    _nclua_get_store (L);                       \
    lua_rawgeti (L, -1, key);                   \
    lua_remove (L, -2);                         \
  }                                             \
  NCLUA_STMT_END

/* Sets store[KEY] to the value at top of stack.
   This macro pops the value from stack.  */
#define _nclua_set_store_data(L, key)           \
  NCLUA_STMT_BEGIN                              \
  {                                             \
    _nclua_get_store (L);                       \
    lua_pushvalue (L, -2);                      \
    lua_rawseti (L, -2, key);                   \
    lua_pop (L, 2);                             \
  }                                             \
  NCLUA_STMT_END

/* nclua.c */

NCLUA_PRIVATE void
_nclua_error (lua_State *L, int level, const char *format, ...);

NCLUA_PRIVATE void
_nclua_warning (lua_State *L, int level, const char *format, ...);

/* nclua-event.c */

NCLUA_PRIVATE nclua_status_t
_nclua_event_open (lua_State *L);

NCLUA_PRIVATE void
_nclua_event_close (lua_State *L);

/* Miscellanea.  */

#undef  FALSE
#define FALSE 0

#undef  TRUE
#define TRUE  1

#undef  likely
#define likely           NCLUA_LIKELY

#undef  unlikely
#define unlikely         NCLUA_UNLIKELY

#undef  arg_unused
#define arg_unused       NCLUA_ARG_UNUSED

#undef  arg_nonnull
#define arg_nonnull      NCLUA_NONNULL

#undef  nelementsof
#define nelementsof(x)   (sizeof (x) / sizeof (x[0]))

#undef  integralof
#define integralof(x)    (((char *)(x)) - ((char *) 0))

#undef  pointerof
#define pointerof(x)     ((void *)((char *) 0 + (x)))

#undef  isodd
#define isodd(n)         ((n) & 1)

#undef  iseven
#define iseven(n)        (!isodd (n))

#undef  sign
#define sign(x)          ((x) >= 0.0 ? 1 : -1)

#undef  max
#define max(a, b)        (((a) > (b)) ? (a) : (b))

#undef  min
#define min(a, b)        (((a) < (b)) ? (a) : (b))

#undef  streq
#define streq(a, b)      ((*(a) == *(b)) && strcmp (a, b) == 0)

#undef  strneq
#define streneq(a, b)    (!streq (a, b))

#undef  DECONST
#define DECONST(t, x)    ((t)(size_t)(const void *)(x))

#undef  DEVOLATILE
#define DEVOLATILE(t, x) ((t)(size_t)(volatile void *)(x))

#undef  DEQUALIFY
#define DEQUALIFY(t, x)  ((t)(size_t)(const volatile void *)(x))

#define _NCLUA_STRINGIFY(arg) #arg
#define NCLUA_STRINGIFY(arg)  _NCLUA_STRINGIFY (arg)

#define _NCLUA_CONCAT(a, b)   a##b
#define NCLUA_CONCAT(a, b)    _NCLUA_CONCAT (a, b)

#define ASSERT_NOT_REACHED    assert (!"reached")

#define _NCLUA_COMPILE_TIME_ASSERT1(cond, line) \
  typedef int tm_compile_time_assertion_at_line_##line##_failed[(cond)?1:-1]
#define _NCLUA_COMPILE_TIME_ASSERT0(cond, line) \
  _NCLUA_COMPILE_TIME_ASSERT1 (cond, line) /* macro-expand LINE */
#define NCLUA_COMPILE_TIME_ASSERT(cond) \
  _NCLUA_COMPILE_TIME_ASSERT0 (cond, __LINE__)

NCLUA_END_DECLS

#endif /* NCLUA_PRIVATE_H */