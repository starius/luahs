// lua-hyperscan, Lua bindings to hyperscan
// Copyright (C) 2015 Boris Nagaev
// See the LICENSE file for terms of use.

#include "hyperscan.h"

int luaopen_hyperscan(lua_State* L) {
    lua_newtable(L); // module "hyperscan"
    createConstantsTable(L);
    lua_setfield(L, -2, "constants");
    return 1;
}
