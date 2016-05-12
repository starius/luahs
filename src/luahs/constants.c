// luahs, Lua bindings to hyperscan
// Copyright (C) 2016 Boris Nagaev
// See the LICENSE file for terms of use.

#include "luahs.h"

typedef struct Constant {
    const char* name;
    int value;
} Constant;

typedef struct Namespace {
    const char* name;
    const Constant* constants;
} Namespace;

#define ITEM(c) {#c, c}

static const Constant errors[] = {
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

static const Constant extended_parameters[] = {
    ITEM(HS_EXT_FLAG_MIN_OFFSET),
    ITEM(HS_EXT_FLAG_MAX_OFFSET),
    ITEM(HS_EXT_FLAG_MIN_LENGTH),
    {},
};

static const Constant pattern_flags[] = {
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

static const Constant cpu_features[] = {
    ITEM(HS_CPU_FEATURES_AVX2),
    {},
};

static const Constant cpu_tuning[] = {
    ITEM(HS_TUNE_FAMILY_GENERIC),
    ITEM(HS_TUNE_FAMILY_SNB),
    ITEM(HS_TUNE_FAMILY_IVB),
    ITEM(HS_TUNE_FAMILY_HSW),
    ITEM(HS_TUNE_FAMILY_SLM),
    ITEM(HS_TUNE_FAMILY_BDW),
    {},
};

static const Constant compile_mode[] = {
    ITEM(HS_MODE_BLOCK),
    ITEM(HS_MODE_NOSTREAM),
    ITEM(HS_MODE_STREAM),
    ITEM(HS_MODE_VECTORED),
    ITEM(HS_MODE_SOM_HORIZON_LARGE),
    ITEM(HS_MODE_SOM_HORIZON_MEDIUM),
    ITEM(HS_MODE_SOM_HORIZON_SMALL),
    {},
};

static const Namespace namespaces[] = {
    ITEM(errors),
    ITEM(extended_parameters),
    ITEM(pattern_flags),
    ITEM(cpu_features),
    ITEM(cpu_tuning),
    ITEM(compile_mode),
    {},
};

#undef ITEM

static void pushConstants(lua_State* L, const Constant* constants) {
    int length = 0;
    const Constant* it;
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

typedef struct Error {
    hs_error_t code;
    const char* name;
} Error;

static const Error error_messages[] = {
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

const char* errorToString(hs_error_t error) {
    const Error* it;
    for (it = error_messages; it->name != NULL; it++) {
        if (error == it->code) {
            return it->name;
        }
    }
    return "Unknown error.";
}

static int decode_error(lua_State* L) {
    int err = luaL_checkinteger(L, 1);
    lua_pushstring(L, errorToString(err));
    return 1;
}

void addConstants(lua_State* L) {
    const Namespace* it;
    for (it = namespaces; it->name != NULL; it++) {
        pushConstants(L, it->constants);
        lua_setfield(L, -2, it->name);
    }
    lua_pushcfunction(L, decode_error);
    lua_setfield(L, -2, "decode_error");
    lua_pushinteger(L, HS_OFFSET_PAST_HORIZON);
    lua_setfield(L, -2, "HS_OFFSET_PAST_HORIZON");
}
