// luahs, Lua bindings to hyperscan
// Copyright (C) 2015 Boris Nagaev
// See the LICENSE file for terms of use.

#ifndef LUA_HS_H_
#define LUA_HS_H_

#include <hs/hs.h>
#include <lua.h>
#include <lauxlib.h>

#if LUA_VERSION_NUM == 501
#define compat_setfuncs(L, funcs) luaL_register(L, 0, funcs)
#else
#define compat_setfuncs(L, funcs) luaL_setfuncs(L, funcs, 0)
#endif

void createConstantsTable(lua_State* L);
const char* errorToString(hs_error_t error);

#endif
