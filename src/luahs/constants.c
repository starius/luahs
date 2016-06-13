// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#include <limits.h>

#include "luahs.h"

typedef struct luahs_Constant {
    const char* name;
    int value;
} luahs_Constant;

typedef struct luahs_Namespace {
    const char* name;
    const luahs_Constant* constants;
} luahs_Namespace;

#define ITEM(c) {#c, c}

static const luahs_Constant luahs_errors[] = {
    ITEM(HS_SUCCESS),
    ITEM(HS_INVALID),
    ITEM(HS_NOMEM),
    ITEM(HS_SCAN_TERMINATED),
    ITEM(HS_COMPILER_ERROR),
    ITEM(HS_DB_VERSION_ERROR),
    ITEM(HS_DB_PLATFORM_ERROR),
    ITEM(HS_DB_MODE_ERROR),
    ITEM(HS_BAD_ALIGN),
    ITEM(HS_BAD_ALLOC),
    {},
};

static const luahs_Constant luahs_extended_parameters[] = {
    ITEM(HS_EXT_FLAG_MIN_OFFSET),
    ITEM(HS_EXT_FLAG_MAX_OFFSET),
    ITEM(HS_EXT_FLAG_MIN_LENGTH),
    {},
};

static const luahs_Constant luahs_pattern_flags[] = {
    ITEM(HS_FLAG_CASELESS),
    ITEM(HS_FLAG_DOTALL),
    ITEM(HS_FLAG_MULTILINE),
    ITEM(HS_FLAG_SINGLEMATCH),
    ITEM(HS_FLAG_ALLOWEMPTY),
    ITEM(HS_FLAG_UTF8),
    ITEM(HS_FLAG_UCP),
    ITEM(HS_FLAG_PREFILTER),
    ITEM(HS_FLAG_SOM_LEFTMOST),
    {},
};

static const luahs_Constant luahs_cpu_features[] = {
    ITEM(HS_CPU_FEATURES_AVX2),
    {},
};

static const luahs_Constant luahs_cpu_tuning[] = {
    ITEM(HS_TUNE_FAMILY_GENERIC),
    ITEM(HS_TUNE_FAMILY_SNB),
    ITEM(HS_TUNE_FAMILY_IVB),
    ITEM(HS_TUNE_FAMILY_HSW),
    ITEM(HS_TUNE_FAMILY_SLM),
    ITEM(HS_TUNE_FAMILY_BDW),
    {},
};

static const luahs_Constant luahs_compile_mode[] = {
    ITEM(HS_MODE_BLOCK),
    ITEM(HS_MODE_NOSTREAM),
    ITEM(HS_MODE_STREAM),
    ITEM(HS_MODE_VECTORED),
    ITEM(HS_MODE_SOM_HORIZON_LARGE),
    ITEM(HS_MODE_SOM_HORIZON_MEDIUM),
    ITEM(HS_MODE_SOM_HORIZON_SMALL),
    {},
};

#undef ITEM

#define NAMESPACE(c) {#c, luahs_ ## c}
static const luahs_Namespace luahs_namespaces[] = {
    NAMESPACE(errors),
    NAMESPACE(extended_parameters),
    NAMESPACE(pattern_flags),
    NAMESPACE(cpu_features),
    NAMESPACE(cpu_tuning),
    NAMESPACE(compile_mode),
    {},
};
#undef NAMESPACE

static void luahs_pushConstants(
    lua_State* L,
    const luahs_Constant* constants
) {
    int length = 0;
    const luahs_Constant* it;
    // determine length
    for (it = constants; it->name != NULL; it++) {
        length += 1;
    }
    lua_createtable(L, 0, length);
    // fill table
    for (it = constants; it->name != NULL; it++) {
        lua_pushinteger(L, it->value);
        lua_setfield(L, -2, it->name);
    }
}

typedef struct luahs_Error {
    hs_error_t code;
    const char* name;
} luahs_Error;

static const luahs_Error luahs_error_messages[] = {
    {HS_SUCCESS, "The engine completed normally."},
    {HS_INVALID, "A parameter passed to this function was invalid."},
    {HS_NOMEM, "A memory allocation failed."},
    {HS_SCAN_TERMINATED, "The engine was terminated by callback."},
    {HS_COMPILER_ERROR, "The pattern compiler failed."},
    {HS_DB_VERSION_ERROR, "The given database was built for "
        "a different version of Hyperscan."},
    {HS_DB_PLATFORM_ERROR, "The given database was built for a "
        "different platform (i.e., CPU type)."},
    {HS_DB_MODE_ERROR, "The given database was built for "
        "a different mode of operation."},
    {HS_BAD_ALIGN, "A parameter passed to this function was "
        "not correctly aligned."},
    {HS_BAD_ALLOC, "The memory allocator (either malloc() or "
        "the allocator set with hs_set_allocator()) did not "
        "correctly return memory suitably aligned for the largest "
        "representable data type on this platform."},
    {},
};

const char* luahs_errorToString(hs_error_t error) {
    const luahs_Error* it;
    for (it = luahs_error_messages; it->name != NULL; it++) {
        if (error == it->code) {
            return it->name;
        }
    }
    return "Unknown error.";
}

static int luahs_decodeError(lua_State* L) {
    int err = luaL_checkinteger(L, 1);
    lua_pushstring(L, luahs_errorToString(err));
    return 1;
}

void luahs_addConstants(lua_State* L) {
    const luahs_Namespace* it;
    for (it = luahs_namespaces; it->name != NULL; it++) {
        luahs_pushConstants(L, it->constants);
        lua_setfield(L, -2, it->name);
    }
    lua_pushcfunction(L, luahs_decodeError);
    lua_setfield(L, -2, "decodeError");
    lua_pushinteger(L, HS_OFFSET_PAST_HORIZON);
    lua_setfield(L, -2, "HS_OFFSET_PAST_HORIZON");
    lua_pushinteger(L, UINT_MAX);
    lua_setfield(L, -2, "UINT_MAX");
}
