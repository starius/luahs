// luahs, Lua bindings to hyperscan
// Copyright (C) 2015 Boris Nagaev
// See the LICENSE file for terms of use.

#include <string.h>

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

static luaL_Reg database_mt_funcs[] = {
    {"__gc", free_database},
    {"__tostring", database_info},
    {}
};

static luaL_Reg database_methods[] = {
    {"info", database_info},
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

typedef lua_Integer (*FilterFunction) (lua_State *L, int index);

static lua_Integer toFlagsInMulti(lua_State* L, int index) {
    return toFlags(L, index, "flags");
}

static int getIntegerField(
    lua_State* L,
    const char* field_name,
    int* result,
    FilterFunction filter
) {
    lua_getfield(L, -1, field_name);
    int id_type = lua_type(L, -1);
    int has_field = 0;
    if (id_type == LUA_TNUMBER ||
            (filter == toFlagsInMulti && id_type == LUA_TTABLE)) {
        *result = filter(L, -1);
        has_field = 1;
    } else if (id_type != LUA_TNIL) {
        return luaL_error(
            L,
            "Bad type of arg1.expressions[i].%s: %s",
            field_name,
            lua_typename(L, id_type)
        );
    }
    lua_pop(L, 1);
    return has_field;
}

static hs_error_t compileMulti(
    lua_State* L,
    Database* self,
    unsigned int mode,
    const hs_platform_info_t* platform,
    hs_compile_error_t** compile_err
) {
    luaL_checktype(L, -1, LUA_TTABLE);
    int nelements = compat_rawlen(L, -1);
    const char** expressions = lua_newuserdata(
        L,
        nelements * sizeof(const char*)
    );
    unsigned int* ids_space = lua_newuserdata(
        L,
        nelements * sizeof(unsigned int)
    );
    memset(ids_space, 0, nelements * sizeof(unsigned int));
    unsigned int* flagss_space = lua_newuserdata(
        L,
        nelements * sizeof(unsigned int)
    );
    memset(flagss_space, 0, nelements * sizeof(unsigned int));
    hs_expr_ext_t* ext_storage = lua_newuserdata(
        L,
        nelements * sizeof(hs_expr_ext_t)
    );
    const hs_expr_ext_t** ext_space = lua_newuserdata(
        L,
        nelements * sizeof(const hs_expr_ext_t*)
    );
    memset(ext_space, 0, nelements * sizeof(const hs_expr_ext_t*));
    unsigned int* ids = NULL;
    unsigned int* flagss = NULL;
    const hs_expr_ext_t** ext = NULL;
    lua_pushvalue(L, -6);
    // table 'expressions' is on top now
    int i;
    for (i = 0; i < nelements; i++) {
        lua_rawgeti(L, -1, i + 1);
        int expression_type = lua_type(L, -1);
        if (expression_type == LUA_TSTRING) {
            expressions[i] = luaL_checkstring(L, -1);
        } else if (expression_type == LUA_TTABLE) {
            // arg1.expressions[i].expression
            lua_getfield(L, -1, "expression");
            expressions[i] = luaL_checkstring(L, -1);
            lua_pop(L, 1);
            // integer fields
            int id;
            if (getIntegerField(L, "id", &id, luaL_checkinteger)) {
                if (!ids) {
                    ids = ids_space;
                }
                ids[i] = id;
            }
            int flags;
            if (getIntegerField(L, "flags", &flags, toFlagsInMulti)) {
                if (!flagss) {
                    flagss = flagss_space;
                }
                flagss[i] = flags;
            }
            // extended flags
            unsigned long long ext_flags = 0;
            int min_offset, max_offset, min_length;
            if (getIntegerField(L, "min_offset", &min_offset, luaL_checkinteger)) {
                ext_flags |= HS_EXT_FLAG_MIN_OFFSET;
            }
            if (getIntegerField(L, "max_offset", &max_offset, luaL_checkinteger)) {
                ext_flags |= HS_EXT_FLAG_MAX_OFFSET;
            }
            if (getIntegerField(L, "min_length", &min_length, luaL_checkinteger)) {
                ext_flags |= HS_EXT_FLAG_MIN_LENGTH;
            }
            if (ext_flags) {
                if (!ext) {
                    ext = ext_space;
                }
                ext[i] = &(ext_storage[i]);
                ext_storage[i].flags = ext_flags;
                ext_storage[i].min_offset = min_offset;
                ext_storage[i].max_offset = max_offset;
                ext_storage[i].min_length = min_length;
            }
        } else {
            return luaL_error(
                L,
                "Bad type of arg1.expressions[i].expression: %s",
                lua_typename(L, expression_type)
            );
        }
        lua_pop(L, 1);
    }
    hs_error_t err;
    if (ext) {
        err = hs_compile_ext_multi(
            expressions,
            flagss,
            ids,
            ext,
            nelements,
            mode,
            platform,
            &self->db,
            compile_err
        );
    } else {
        err = hs_compile_multi(
            expressions,
            flagss,
            ids,
            nelements,
            mode,
            platform,
            &self->db,
            compile_err
        );
    }
    lua_pop(L, 1); // arg1.expressions (copy)
    lua_pop(L, 1); // ext_space (userdata)
    lua_pop(L, 1); // ext_storage (userdata)
    lua_pop(L, 1); // flagss_space (userdata)
    lua_pop(L, 1); // ids_space (userdata)
    lua_pop(L, 1); // expressions (userdata)
    return err;
}

static int compile(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
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
    Database* self = createDatabase(L);
    int result = lua_gettop(L);
    // compile
    hs_compile_error_t* compile_err;
    hs_error_t err;
    int compiled = 0;
    if (!compiled) {
        // single expression
        lua_getfield(L, 1, "expression");
        if (lua_type(L, -1) != LUA_TNIL) {
            const char* expression = luaL_checkstring(L, -1);
            // flags
            lua_getfield(L, 1, "flags");
            unsigned int flags = toFlags(L, -1, "flags");
            lua_pop(L, 1);
            err = hs_compile(
                expression,
                flags,
                mode,
                platform,
                &self->db,
                &compile_err
            );
            compiled = 1;
        }
        lua_pop(L, 1);
    }
    if (!compiled) {
        // multiple expressions
        lua_getfield(L, 1, "expressions");
        if (lua_type(L, -1) != LUA_TNIL) {
            err = compileMulti(L, self, mode, platform, &compile_err);
            compiled = 1;
        }
        lua_pop(L, 1);
    }
    if (!compiled) {
        return luaL_error(L, "Specify 'expression' or 'expressions'");
    }
    if (err != HS_SUCCESS) {
        lua_pushfstring(L, "Unable to compile: %s", compile_err->message);
        hs_free_compile_error(compile_err);
        return lua_error(L);
    }
    lua_pushvalue(L, result);
    return 1;
}

static int current_platform(lua_State* L) {
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
    ITEM(current_platform),
    ITEM(compile),
    {}
};

#undef ITEM

void addFunctions(lua_State* L) {
    compat_setfuncs(L, functions);
}
