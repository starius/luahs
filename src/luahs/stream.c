// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#include <string.h>

#include "luahs.h"

static int free_stream(lua_State* L) {
    Stream* self = luaL_checkudata(L, 1, STREAM_MT);
    if (self->stream) {
        hs_error_t err = hs_close_stream(self->stream, NULL, NULL, NULL);
        if (err != HS_SUCCESS) {
            return luaL_error(L, errorToString(err));
        }
    }
    self->stream = NULL;
    luaL_unref(L, LUA_REGISTRYINDEX, self->db_ref);
    self->db_ref = LUA_NOREF;
    return 0;
}

static int stream_tostring(lua_State* L) {
    Stream* self = luaL_checkudata(L, 1, STREAM_MT);
    lua_pushfstring(
        L,
        "Hyperscan stream (%p)",
        self->stream
    );
    return 1;
}

static int clone_stream(lua_State* L) {
    Stream* self = luaL_checkudata(L, 1, STREAM_MT);
    Stream* copy = createStream(L);
    hs_error_t err = hs_copy_stream(&copy->stream, self->stream);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    lua_rawgeti(L, LUA_REGISTRYINDEX, self->db_ref);
    copy->db_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return 1;
}

static int getDatabase(lua_State* L) {
    Stream* self = luaL_checkudata(L, 1, STREAM_MT);
    lua_rawgeti(L, LUA_REGISTRYINDEX, self->db_ref);
    return 1;
}

static const luaL_Reg stream_mt_funcs[] = {
    {"__gc", free_stream},
    {"__tostring", stream_tostring},
    {}
};

static const luaL_Reg stream_methods[] = {
    {"scan", scanAgainstStream},
    {"clone", clone_stream},
    {"close", closeStream},
    {"database", getDatabase},
    {}
};

Stream* createStream(lua_State* L) {
    Stream* self = lua_newuserdata(L, sizeof(Stream));
    self->stream = NULL;
    self->db_ref = LUA_NOREF;
    if (luaL_newmetatable(L, STREAM_MT)) {
        // prepare metatable
        compat_setfuncs(L, stream_mt_funcs);
        lua_newtable(L);
        compat_setfuncs(L, stream_methods);
        lua_setfield(L, -2, "__index");
    }
    lua_setmetatable(L, -2);
    return self;
}

int makeStream(lua_State* L) {
    Database* db = luaL_checkudata(L, 1, DATABASE_MT);
    Stream* self = createStream(L);
    int flags = 0;
    hs_error_t err = hs_open_stream(db->db, flags, &self->stream);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    lua_pushvalue(L, 1);
    self->db_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return 1;
}
