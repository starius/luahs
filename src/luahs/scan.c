// luahs, Lua bindings to hyperscan
// Copyright (C) 2015 Boris Nagaev
// See the LICENSE file for terms of use.

#include <string.h>

#include "luahs.h"

static int luahs_match_event_handler(
    unsigned int id,
    unsigned long long from,
    unsigned long long to,
    unsigned int flags,
    void* context
) {
    MatchContext* match_context = (MatchContext*)context;
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

int scanAgainstDatabase(lua_State* L) {
    Database* db = luaL_checkudata(L, 1, DATABASE_MT);
    size_t length;
    const char* data = luaL_checklstring(L, 2, &length);
    Scratch* scratch = luaL_checkudata(L, 3, SCRATCH_MT);
    lua_newtable(L);
    int results_table = lua_gettop(L);
    MatchContext match_context = {
        .L = L,
        .results_table = results_table,
        .nresults = 0,
    };
    int flags; // unused
    hs_error_t err = hs_scan(
        db->db,
        data,
        length,
        flags,
        scratch->scratch,
        luahs_match_event_handler,
        &match_context
    );
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    return 1;
}
