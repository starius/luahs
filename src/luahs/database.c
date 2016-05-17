// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#include "luahs.h"

static int luahs_freeDatabase(lua_State* L) {
    luahs_Database* self = luaL_checkudata(L, 1, LUAHS_DATABASE_MT);
    hs_error_t err = hs_free_database(self->db);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    return 0;
}

static int luahs_databaseInfo(lua_State* L) {
    luahs_Database* self = luaL_checkudata(L, 1, LUAHS_DATABASE_MT);
    char* info;
    hs_error_t err = hs_database_info(self->db, &info);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    lua_pushstring(L, info);
    free(info);
    return 1;
}

static int luahs_databaseToString(lua_State* L) {
    luahs_Database* self = luaL_checkudata(L, 1, LUAHS_DATABASE_MT);
    char* info;
    hs_error_t err = hs_database_info(self->db, &info);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    lua_pushfstring(
        L,
        "Hyperscan database (%p): %s",
        self->db,
        info
    );
    free(info);
    return 1;
}

static int luahs_serializeDatabase(lua_State* L) {
    luahs_Database* self = luaL_checkudata(L, 1, LUAHS_DATABASE_MT);
    char* bytes;
    size_t length;
    hs_error_t err = hs_serialize_database(self->db, &bytes, &length);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    lua_pushlstring(L, bytes, length);
    free(bytes);
    return 1;
}

static int luahs_databaseSize(lua_State* L) {
    luahs_Database* self = luaL_checkudata(L, 1, LUAHS_DATABASE_MT);
    size_t database_size;
    hs_error_t err = hs_database_size(self->db, &database_size);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    lua_pushinteger(L, (int)database_size);
    return 1;
}

static int luahs_streamSize(lua_State* L) {
    luahs_Database* self = luaL_checkudata(L, 1, LUAHS_DATABASE_MT);
    size_t stream_size;
    hs_error_t err = hs_stream_size(self->db, &stream_size);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    lua_pushinteger(L, (int)stream_size);
    return 1;
}

static const luaL_Reg luahs_database_mt_funcs[] = {
    {"__gc", luahs_freeDatabase},
    {"__tostring", luahs_databaseToString},
    {}
};

static const luaL_Reg luahs_database_methods[] = {
    {"info", luahs_databaseInfo},
    {"serialize", luahs_serializeDatabase},
    {"size", luahs_databaseSize},
    {"streamSize", luahs_streamSize},
    {"makeScratch", luahs_makeScratch},
    {"makeStream", luahs_makeStream},
    {"scan", luahs_scanAgainstDatabase},
    {}
};

luahs_Database* luahs_createDatabase(lua_State* L) {
    luahs_Database* self = lua_newuserdata(L, sizeof(luahs_Database));
    self->db = NULL;
    if (luaL_newmetatable(L, LUAHS_DATABASE_MT)) {
        // prepare metatable
        luahs_setfuncs(L, luahs_database_mt_funcs);
        lua_newtable(L);
        luahs_setfuncs(L, luahs_database_methods);
        lua_setfield(L, -2, "__index");
    }
    lua_setmetatable(L, -2);
    return self;
}

static int luahs_deserializeDatabase(lua_State* L) {
    size_t length;
    const char* bytes = luaL_checklstring(L, 1, &length);
    luahs_Database* self = luahs_createDatabase(L);
    hs_error_t err = hs_deserialize_database(bytes, length, &self->db);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    return 1;
}

static int luahs_sizeOfDeserialized(lua_State* L) {
    size_t length;
    const char* bytes = luaL_checklstring(L, 1, &length);
    size_t deserialized_size;
    hs_error_t err = hs_serialized_database_size(
        bytes,
        length,
        &deserialized_size
    );
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    lua_pushinteger(L, (int)deserialized_size);
    return 1;
}

static int luahs_infoOfDeserialized(lua_State* L) {
    size_t length;
    const char* bytes = luaL_checklstring(L, 1, &length);
    char* info;
    hs_error_t err = hs_serialized_database_info(
        bytes,
        length,
        &info
    );
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    lua_pushstring(L, info);
    free(info);
    return 1;
}

static luaL_Reg luahs_functions[] = {
    {"deserialize", luahs_deserializeDatabase},
    {"sizeOfDeserialized", luahs_sizeOfDeserialized},
    {"infoOfDeserialized", luahs_infoOfDeserialized},
    {}
};

void luahs_addDatabase(lua_State* L) {
    luahs_setfuncs(L, luahs_functions);
}
