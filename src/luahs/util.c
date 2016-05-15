// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#include "luahs.h"

static int luahs_version(lua_State* L) {
    lua_pushstring(L, hs_version());
    return 1;
}

static const luaL_Reg luahs_functions[] = {
    {"version", luahs_version},
    {}
};

void luahs_addUtil(lua_State* L) {
    luahs_setfuncs(L, luahs_functions);
}
