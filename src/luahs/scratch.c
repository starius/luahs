// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#include "luahs.h"

static int luahs_freeScratch(lua_State* L) {
    luahs_Scratch* self = luaL_checkudata(L, 1, LUAHS_SCRATCH_MT);
    hs_error_t err = hs_free_scratch(self->scratch);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    return 0;
}

static int luahs_scratchToString(lua_State* L) {
    luahs_Scratch* self = luaL_checkudata(L, 1, LUAHS_SCRATCH_MT);
    size_t size;
    hs_error_t err = hs_scratch_size(self->scratch, &size);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    lua_pushfstring(
        L,
        "Hyperscan scratch (%p) of size %d",
        self->scratch,
        (int)size
    );
    return 1;
}

static int luahs_scratchSize(lua_State* L) {
    luahs_Scratch* self = luaL_checkudata(L, 1, LUAHS_SCRATCH_MT);
    size_t size;
    hs_error_t err = hs_scratch_size(self->scratch, &size);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    lua_pushinteger(L, size);
    return 1;
}

static int luahs_growScratch(lua_State* L) {
    luahs_Scratch* self = luaL_checkudata(L, 1, LUAHS_SCRATCH_MT);
    luahs_Database* db = luaL_checkudata(L, 2, LUAHS_DATABASE_MT);
    // self->scratch is not NULL
    hs_error_t err = hs_alloc_scratch(db->db, &self->scratch);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    return 0;
}

static int luahs_cloneScratch(lua_State* L) {
    luahs_Scratch* self = luaL_checkudata(L, 1, LUAHS_SCRATCH_MT);
    luahs_Scratch* copy = luahs_createScratch(L);
    hs_error_t err = hs_clone_scratch(self->scratch, &copy->scratch);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    return 1;
}

static const luaL_Reg luahs_scratch_mt_funcs[] = {
    {"__gc", luahs_freeScratch},
    {"__tostring", luahs_scratchToString},
    {}
};

static const luaL_Reg luahs_scratch_methods[] = {
    {"size", luahs_scratchSize},
    {"grow", luahs_growScratch},
    {"clone", luahs_cloneScratch},
    {}
};

luahs_Scratch* luahs_createScratch(lua_State* L) {
    luahs_Scratch* self = lua_newuserdata(L, sizeof(luahs_Scratch));
    self->scratch = NULL;
    if (luaL_newmetatable(L, LUAHS_SCRATCH_MT)) {
        // prepare metatable
        luahs_setfuncs(L, luahs_scratch_mt_funcs);
        lua_newtable(L);
        luahs_setfuncs(L, luahs_scratch_methods);
        lua_setfield(L, -2, "__index");
    }
    lua_setmetatable(L, -2);
    return self;
}

int luahs_makeScratch(lua_State* L) {
    luahs_Database* db = luaL_checkudata(L, 1, LUAHS_DATABASE_MT);
    luahs_Scratch* self = luahs_createScratch(L);
    // self->scratch is NULL
    hs_error_t err = hs_alloc_scratch(db->db, &self->scratch);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    return 1;
}
