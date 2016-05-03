// luahs, Lua bindings to hyperscan
// Copyright (C) 2015 Boris Nagaev
// See the LICENSE file for terms of use.

#include "luahs.h"

int luaopen_luahs(lua_State* L) {
    lua_newtable(L); // module "luahs"
    addConstants(L);
    addFunctions(L);
    return 1;
}
