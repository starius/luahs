// luahs, Lua bindings to hyperscan
// Copyright (C) 2015 Boris Nagaev
// See the LICENSE file for terms of use.

#include "luahs.h"

typedef struct Database {
    hs_database_t* db;
} Database;

static int free_database(lua_State* L) {
    Database* self = luaL_checkudata(L, 1, DATABASE_MT);
    hs_error_t err = hs_free_database(self->db);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    return 0;
}

static luaL_Reg database_mt_funcs[] = {
    {"__gc", free_database},
    {}
};

static luaL_Reg database_methods[] = {
    {}
};

static int toFlags(lua_State* L, int index, const char* name) {
    // flags can be provided as integer or as a table of integers
    int flags = 0;
    int flags_type = lua_type(L, index);
    if (flags_type == LUA_TNUMBER) {
        flags = luaL_checkinteger(L, index);
    } else if (flags_type == LUA_TTABLE) {
        flags = 0;
        int flast_length = compat_rawlen(L, index);
        int i;
        for (i = 0; i < flast_length; i++) {
            lua_rawgeti(L, index, i + 1);
            flags |= luaL_checkinteger(L, -1);
            lua_pop(L, 1);
        }
    } else if (flags_type != LUA_TNIL) {
        return luaL_error(
            L,
            "Bad type of '%s': %s",
            name, lua_typename(L, flags_type)
        );
    }
    return flags;
}

static int toMode(lua_State* L, int index) {
    int mode;
    int mode_type = lua_type(L, index);
    if (mode_type == LUA_TNUMBER) {
        mode = luaL_checkinteger(L, index);
    } else {
        return luaL_error(
            L,
            "Bad type of 'mode': %s",
            lua_typename(L, mode_type)
        );
    }
    return mode;
}

static const hs_platform_info_t* toPlatform(
    lua_State* L,
    int index,
    hs_platform_info_t* space
) {
    hs_platform_info_t* platform = NULL;
    int platform_type = lua_type(L, index);
    if (platform_type == LUA_TTABLE) {
        platform = space;
        lua_getfield(L, index, "tune");
        platform->tune = toFlags(L, -1, "tune");
        lua_pop(L, 1);
        //
        lua_getfield(L, index, "cpu_features");
        platform->cpu_features = toFlags(L, -1, "cpu_features");
        lua_pop(L, 1);
        //
        lua_getfield(L, index, "reserved1");
        platform->reserved1 = toFlags(L, -1, "reserved1");
        lua_pop(L, 1);
        //
        lua_getfield(L, index, "reserved2");
        platform->reserved2 = toFlags(L, -1, "reserved2");
        lua_pop(L, 1);
    } else if (platform_type != LUA_TNIL) {
        luaL_error(
            L,
            "Bad type of 'platform': %s",
            lua_typename(L, platform_type)
        );
        return NULL; // unreachable
    }
    return platform;
}

static int compile(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    // flags
    lua_getfield(L, 1, "flags");
    unsigned int flags = toFlags(L, -1, "flags");
    lua_pop(L, 1);
    // mode
    lua_getfield(L, 1, "mode");
    unsigned int mode = toMode(L, -1);
    lua_pop(L, 1);
    // platform
    lua_getfield(L, 1, "platform");
    hs_platform_info_t space;
    const hs_platform_info_t* platform = toPlatform(L, -1, &space);
    lua_pop(L, 1);
    // create userdata
    Database* self = lua_newuserdata(L, sizeof(Database));
    int result = lua_gettop(L);
    self->db = NULL;
    if (luaL_newmetatable(L, DATABASE_MT)) {
        // prepare metatable
        compat_setfuncs(L, database_mt_funcs);
        lua_newtable(L);
        compat_setfuncs(L, database_methods);
        lua_setfield(L, -2, "__index");
    }
    lua_setmetatable(L, -2);
    // compile
    hs_compile_error_t* compile_err;
    // single expression case
    lua_getfield(L, 1, "expression");
    if (lua_type(L, -1) != LUA_TNIL) {
        const char* expression = luaL_checkstring(L, -1);
        hs_error_t err = hs_compile(
            expression,
            flags,
            mode,
            platform,
            &self->db,
            &compile_err
        );
        if (err != HS_SUCCESS) {
            lua_pushfstring(
                L,
                "Unable to compile pattern '%s': %s",
                expression,
                compile_err->message
            );
            hs_free_compile_error(compile_err);
            return lua_error(L);
        }
        lua_pushvalue(L, result);
        return 1;
    }
    lua_pop(L, 1);
    return 0; // TODO: implement other compile functions
}

static int populate_platform(lua_State* L) {
    hs_platform_info_t plat;
    hs_error_t err = hs_populate_platform(&plat);
    if (err != HS_SUCCESS) {
        return luaL_error(L, errorToString(err));
    }
    lua_createtable(L, 0, 4);
    lua_pushinteger(L, plat.tune);
    lua_setfield(L, -2, "tune");
    lua_pushinteger(L, plat.cpu_features);
    lua_setfield(L, -2, "cpu_features");
    lua_pushinteger(L, plat.reserved1);
    lua_setfield(L, -2, "reserved1");
    lua_pushinteger(L, plat.reserved2);
    lua_setfield(L, -2, "reserved2");
    return 1;
}

#define ITEM(c) {#c, c}

static luaL_Reg functions[] = {
    ITEM(populate_platform),
    ITEM(compile),
    {}
};

#undef ITEM

void createFunctionsTable(lua_State* L) {
    int length = 0;
    const luaL_Reg* it;
    // determine length
    for (it = functions; it->name != NULL; it++) {
        length += 1;
    }
    lua_createtable(L, 0, length);
    // fill table
    compat_setfuncs(L, functions);
}
