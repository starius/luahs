// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#include <string.h>

#include "luahs.h"

static int free_scratch(lua_State* L) {
    Scratch* self = luaL_checkudata(L, 1, SCRATCH_MT);
    hs_error_t err = hs_free_scratch(self->scratch);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    return 0;
}

static int scratch_size(lua_State* L) {
    Scratch* self = luaL_checkudata(L, 1, SCRATCH_MT);
    size_t size;
    hs_error_t err = hs_scratch_size(self->scratch, &size);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    lua_pushinteger(L, size);
    return 1;
}

static int grow_scratch(lua_State* L) {
    Scratch* self = luaL_checkudata(L, 1, SCRATCH_MT);
    Database* db = luaL_checkudata(L, 2, DATABASE_MT);
    // self->scratch is not NULL
    hs_error_t err = hs_alloc_scratch(db->db, &self->scratch);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    return 0;
}

static int clone_scratch(lua_State* L) {
    Scratch* self = luaL_checkudata(L, 1, SCRATCH_MT);
    Scratch* copy = createScratch(L);
    hs_error_t err = hs_clone_scratch(self->scratch, &copy->scratch);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    return 1;
}

static const luaL_Reg scratch_mt_funcs[] = {
    {"__gc", free_scratch},
    {}
};

static const luaL_Reg scratch_methods[] = {
    {"size", scratch_size},
    {"grow", grow_scratch},
    {"clone", clone_scratch},
    {}
};

Scratch* createScratch(lua_State* L) {
    Scratch* self = lua_newuserdata(L, sizeof(Scratch));
    self->scratch = NULL;
    if (luaL_newmetatable(L, SCRATCH_MT)) {
        // prepare metatable
        compat_setfuncs(L, scratch_mt_funcs);
        lua_newtable(L);
        compat_setfuncs(L, scratch_methods);
        lua_setfield(L, -2, "__index");
    }
    lua_setmetatable(L, -2);
    return self;
}

int makeScratch(lua_State* L) {
    Database* db = luaL_checkudata(L, 1, DATABASE_MT);
    Scratch* self = createScratch(L);
    // self->scratch is NULL
    hs_error_t err = hs_alloc_scratch(db->db, &self->scratch);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    return 1;
}
