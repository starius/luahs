// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#ifndef LUA_HS_H_
#define LUA_HS_H_

#include <hs/hs.h>
#include <lua.h>
#include <lauxlib.h>

#if LUA_VERSION_NUM == 501
#define compat_setfuncs(L, funcs) luaL_register(L, 0, funcs)
#define compat_rawlen lua_objlen
#else
#define compat_setfuncs(L, funcs) luaL_setfuncs(L, funcs, 0)
#define compat_rawlen lua_rawlen
#endif

#define DATABASE_MT "hs_Database"
#define SCRATCH_MT "hs_Scratch"
#define STREAM_MT "hs_Stream"

// for luaL_Reg
#define ITEM(c) {#c, c}

typedef struct Database {
    hs_database_t* db;
} Database;

typedef struct Scratch {
    hs_scratch_t* scratch;
} Scratch;

typedef struct Stream {
    hs_stream_t* stream;
    int db_ref;
} Stream;

typedef struct MatchContext {
    lua_State* L;
    int results_table;
    int nresults;
} MatchContext;

Database* createDatabase(lua_State* L);
Scratch* createScratch(lua_State* L);
Stream* createStream(lua_State* L);

void addConstants(lua_State* L);
void addUtil(lua_State* L);
void addDatabase(lua_State* L);
void addCompile(lua_State* L);
const char* errorToString(hs_error_t error);

int makeScratch(lua_State* L);
int makeStream(lua_State* L);
int scanAgainstDatabase(lua_State* L);
int scanAgainstStream(lua_State* L);
int closeStream(lua_State* L);

#endif
