// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#include "luahs.h"

#ifdef LUAHS_RUNTIME
#define LUAHS_LUAOPENER luaopen_luahs_runtime
#else
#define LUAHS_LUAOPENER luaopen_luahs
#endif

int LUAHS_LUAOPENER(lua_State* L) {
    lua_newtable(L); // module "luahs"
    addConstants(L);
    addUtil(L);
#ifndef LUAHS_RUNTIME
    addCompile(L);
#endif
    addDatabase(L);
    return 1;
}
