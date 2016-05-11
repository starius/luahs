// luahs, Lua bindings to hyperscan
// Copyright (C) 2015 Boris Nagaev
// See the LICENSE file for terms of use.

#include <string.h>

#include "luahs.h"

static int version(lua_State* L) {
    lua_pushstring(L, hs_version());
    return 1;
}

static const luaL_Reg functions[] = {
    ITEM(version),
    {}
};

void addUtil(lua_State* L) {
    compat_setfuncs(L, functions);
}
