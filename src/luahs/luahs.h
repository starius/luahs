// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#ifndef LUA_HS_H_
#define LUA_HS_H_

#include <hs/hs.h>
#include <lua.h>
#include <lauxlib.h>

#if LUA_VERSION_NUM == 501
#define luahs_setfuncs(L, funcs) luaL_register(L, 0, funcs)
#define luahs_rawlen lua_objlen
#else
#define luahs_setfuncs(L, funcs) luaL_setfuncs(L, funcs, 0)
#define luahs_rawlen lua_rawlen
#endif

#define LUAHS_DATABASE_MT "hs_Database"
#define LUAHS_SCRATCH_MT "hs_Scratch"
#define LUAHS_STREAM_MT "hs_Stream"

typedef struct luahs_Database {
    hs_database_t* db;
} luahs_Database;

typedef struct luahs_Scratch {
    hs_scratch_t* scratch;
} luahs_Scratch;

typedef struct luahs_Stream {
    hs_stream_t* stream;
    int db_ref;
} luahs_Stream;

typedef struct luahs_MatchContext {
    lua_State* L;
    int results_table;
    int nresults;
} luahs_MatchContext;

luahs_Database* luahs_createDatabase(lua_State* L);
luahs_Scratch* luahs_createScratch(lua_State* L);
luahs_Stream* luahs_createStream(lua_State* L);

void luahs_addConstants(lua_State* L);
void luahs_addUtil(lua_State* L);
void luahs_addDatabase(lua_State* L);
void luahs_addCompile(lua_State* L);
const char* luahs_errorToString(hs_error_t error);

int luahs_makeScratch(lua_State* L);
int luahs_makeStream(lua_State* L);
int luahs_scanAgainstDatabase(lua_State* L);
int luahs_scanAgainstStream(lua_State* L);
int luahs_closeStream(lua_State* L);
int luahs_resetStream(lua_State* L);

#endif
