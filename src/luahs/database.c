// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#include <string.h>

#include "luahs.h"

static int free_database(lua_State* L) {
    Database* self = luaL_checkudata(L, 1, DATABASE_MT);
    hs_error_t err = hs_free_database(self->db);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    return 0;
}

static int database_info(lua_State* L) {
    Database* self = luaL_checkudata(L, 1, DATABASE_MT);
    char* info;
    hs_error_t err = hs_database_info(self->db, &info);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    lua_pushstring(L, info);
    free(info);
    return 1;
}

static int database_serialize(lua_State* L) {
    Database* self = luaL_checkudata(L, 1, DATABASE_MT);
    char* bytes;
    size_t length;
    hs_error_t err = hs_serialize_database(self->db, &bytes, &length);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    lua_pushlstring(L, bytes, length);
    free(bytes);
    return 1;
}

static const luaL_Reg database_mt_funcs[] = {
    {"__gc", free_database},
    {"__tostring", database_info},
    {}
};

static const luaL_Reg database_methods[] = {
    {"info", database_info},
    {"serialize", database_serialize},
    {"makeScratch", makeScratch},
    {"makeStream", makeStream},
    {"scan", scanAgainstDatabase},
    {}
};

Database* createDatabase(lua_State* L) {
    Database* self = lua_newuserdata(L, sizeof(Database));
    self->db = NULL;
    if (luaL_newmetatable(L, DATABASE_MT)) {
        // prepare metatable
        compat_setfuncs(L, database_mt_funcs);
        lua_newtable(L);
        compat_setfuncs(L, database_methods);
        lua_setfield(L, -2, "__index");
    }
    lua_setmetatable(L, -2);
    return self;
}

static int deserialize(lua_State* L) {
    size_t length;
    const char* bytes = luaL_checklstring(L, 1, &length);
    Database* self = createDatabase(L);
    hs_error_t err = hs_deserialize_database(bytes, length, &self->db);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    return 1;
}

static luaL_Reg functions[] = {
    ITEM(deserialize),
    {}
};

void addDatabase(lua_State* L) {
    compat_setfuncs(L, functions);
}
