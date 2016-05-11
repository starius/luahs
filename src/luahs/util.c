// luahs, Lua bindings to hyperscan
// Copyright (C) 2015 Boris Nagaev
// See the LICENSE file for terms of use.

#include <string.h>

#include "luahs.h"

static int current_platform(lua_State* L) {
    hs_platform_info_t plat;
    hs_error_t err = hs_populate_platform(&plat);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    lua_createtable(L, 0, 4);
    lua_pushinteger(L, plat.tune);
    lua_setfield(L, -2, "tune");
    lua_pushinteger(L, plat.cpu_features);
    lua_setfield(L, -2, "cpu_features");
    lua_pushinteger(L, plat.reserved1);
    lua_setfield(L, -2, "reserved1");
    lua_pushinteger(L, plat.reserved2);
    lua_setfield(L, -2, "reserved2");
    return 1;
}

static int version(lua_State* L) {
    lua_pushstring(L, hs_version());
    return 1;
}

static const luaL_Reg functions[] = {
    ITEM(current_platform),
    ITEM(version),
    {}
};

void addUtil(lua_State* L) {
    compat_setfuncs(L, functions);
}
