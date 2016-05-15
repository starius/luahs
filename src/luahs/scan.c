// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#include "luahs.h"

static int luahs_match_event_handler(
    unsigned int id,
    unsigned long long from,
    unsigned long long to,
    unsigned int flags,
    void* context
) {
    luahs_MatchContext* match_context = (luahs_MatchContext*)context;
    lua_State* L = match_context->L;
    lua_createtable(L, 0, 3);
    lua_pushinteger(L, id);
    lua_setfield(L, -2, "id");
    lua_pushinteger(L, from);
    lua_setfield(L, -2, "from");
    lua_pushinteger(L, to);
    lua_setfield(L, -2, "to");
    match_context->nresults += 1;
    lua_rawseti(
        L,
        match_context->results_table,
        match_context->nresults
    );
    return 0;
}

int luahs_scanAgainstDatabase(lua_State* L) {
    luahs_Database* db = luaL_checkudata(L, 1, LUAHS_DATABASE_MT);
    luahs_Scratch* scratch = luaL_checkudata(L, 3, LUAHS_SCRATCH_MT);
    lua_newtable(L);
    int results_table = lua_gettop(L);
    luahs_MatchContext match_context = {
        .L = L,
        .results_table = results_table,
        .nresults = 0,
    };
    int flags = 0; // unused
    int data_type = lua_type(L, 2);
    hs_error_t err;
    if (data_type == LUA_TSTRING) {
        size_t length;
        const char* data = luaL_checklstring(L, 2, &length);
        err = hs_scan(
            db->db,
            data,
            length,
            flags,
            scratch->scratch,
            luahs_match_event_handler,
            &match_context
        );
    } else if (data_type == LUA_TTABLE) {
        int count = luahs_rawlen(L, 2);
        size_t item_size = sizeof(const char*) + sizeof(unsigned int);
        void* items = lua_newuserdata(L, count * item_size);
        const char** data = items;
        unsigned int* length = (unsigned int*)(data + count);
        int i;
        for (i = 0; i < count; i++) {
            lua_rawgeti(L, 2, i + 1);
            size_t len;
            data[i] = luaL_checklstring(L, -1, &len);
            length[i] = (int)len;
            lua_pop(L, 1);
        }
        err = hs_scan_vector(
            db->db,
            data,
            length,
            count,
            flags,
            scratch->scratch,
            luahs_match_event_handler,
            &match_context
        );
        lua_pop(L, 1); // items
    } else {
        return luaL_error(
            L,
            "Bad type of 'data': %s",
            lua_typename(L, data_type)
        );
    }
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    return 1;
}

int luahs_scanAgainstStream(lua_State* L) {
    luahs_Stream* stream = luaL_checkudata(L, 1, LUAHS_STREAM_MT);
    size_t length;
    const char* data = luaL_checklstring(L, 2, &length);
    luahs_Scratch* scratch = luaL_checkudata(L, 3, LUAHS_SCRATCH_MT);
    lua_newtable(L);
    int results_table = lua_gettop(L);
    luahs_MatchContext match_context = {
        .L = L,
        .results_table = results_table,
        .nresults = 0,
    };
    int flags = 0; // unused
    hs_error_t err = hs_scan_stream(
        stream->stream,
        data,
        length,
        flags,
        scratch->scratch,
        luahs_match_event_handler,
        &match_context
    );
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    return 1;
}

int luahs_closeStream(lua_State* L) {
    luahs_Stream* stream = luaL_checkudata(L, 1, LUAHS_STREAM_MT);
    luahs_Scratch* scratch = luaL_checkudata(L, 2, LUAHS_SCRATCH_MT);
    lua_newtable(L);
    int results_table = lua_gettop(L);
    luahs_MatchContext match_context = {
        .L = L,
        .results_table = results_table,
        .nresults = 0,
    };
    hs_error_t err = hs_close_stream(
        stream->stream,
        scratch->scratch,
        luahs_match_event_handler,
        &match_context
    );
    if (err != HS_SUCCESS) {
        return luaL_error(L, luahs_errorToString(err));
    }
    stream->stream = NULL;
    luaL_unref(L, LUA_REGISTRYINDEX, stream->db_ref);
    stream->db_ref = LUA_NOREF;
    return 1;
}
