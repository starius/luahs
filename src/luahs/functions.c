// luahs, Lua bindings to hyperscan
// Copyright (C) 2015 Boris Nagaev
// See the LICENSE file for terms of use.

#include "luahs.h"

static int populate_platform(lua_State* L) {
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

#define ITEM(c) {#c, c}

static luaL_Reg functions[] = {
    ITEM(populate_platform),
    {}
};

#undef ITEM

void createFunctionsTable(lua_State* L) {
    int length = 0;
    const luaL_Reg* it;
    // determine length
    for (it = functions; it->name != NULL; it++) {
        length += 1;
    }
    lua_createtable(L, 0, length);
    // fill table
    compat_setfuncs(L, functions);
}
