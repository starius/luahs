// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#include "luahs.h"

static int luahs_freeStream(lua_State* L) {
    luahs_Stream* self = luaL_checkudata(L, 1, LUAHS_STREAM_MT);
    if (self->stream) {
        hs_error_t err = hs_close_stream(self->stream, NULL, NULL, NULL);
        if (err != HS_SUCCESS) {
            return luaL_error(L, luahs_errorToString(err));
        }
    }
    self->stream = NULL;
    luaL_unref(L, LUA_REGISTRYINDEX, self->db_ref);
    self->db_ref = LUA_NOREF;
    return 0;
}

static int luahs_streamToString(lua_State* L) {
    luahs_Stream* self = luaL_checkudata(L, 1, LUAHS_STREAM_MT);
    lua_pushfstring(
        L,
        "Hyperscan stream (%p)",
        self->stream
    );
    return 1;
}

static int luahs_cloneStream(lua_State* L) {
    luahs_Stream* self = luaL_checkudata(L, 1, LUAHS_STREAM_MT);
    luahs_Stream* copy = luahs_createStream(L);
    hs_error_t err = hs_copy_stream(&copy->stream, self->stream);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    lua_rawgeti(L, LUA_REGISTRYINDEX, self->db_ref);
    copy->db_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return 1;
}

static int luahs_getDatabase(lua_State* L) {
    luahs_Stream* self = luaL_checkudata(L, 1, LUAHS_STREAM_MT);
    lua_rawgeti(L, LUA_REGISTRYINDEX, self->db_ref);
    return 1;
}

static const luaL_Reg luahs_stream_mt_funcs[] = {
    {"__gc", luahs_freeStream},
    {"__tostring", luahs_streamToString},
    {}
};

static const luaL_Reg luahs_stream_methods[] = {
    {"scan", luahs_scanAgainstStream},
    {"clone", luahs_cloneStream},
    {"close", luahs_closeStream},
    {"database", luahs_getDatabase},
    {}
};

luahs_Stream* luahs_createStream(lua_State* L) {
    luahs_Stream* self = lua_newuserdata(L, sizeof(luahs_Stream));
    self->stream = NULL;
    self->db_ref = LUA_NOREF;
    if (luaL_newmetatable(L, LUAHS_STREAM_MT)) {
        // prepare metatable
        luahs_setfuncs(L, luahs_stream_mt_funcs);
        lua_newtable(L);
        luahs_setfuncs(L, luahs_stream_methods);
        lua_setfield(L, -2, "__index");
    }
    lua_setmetatable(L, -2);
    return self;
}

int luahs_makeStream(lua_State* L) {
    luahs_Database* db = luaL_checkudata(L, 1, LUAHS_DATABASE_MT);
    luahs_Stream* self = luahs_createStream(L);
    int flags = 0;
    hs_error_t err = hs_open_stream(db->db, flags, &self->stream);
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    lua_pushvalue(L, 1);
    self->db_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return 1;
}
