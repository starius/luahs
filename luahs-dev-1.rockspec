-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2015 Boris Nagaev
-- See the LICENSE file for terms of use.

package = "luahs"
version = "dev-1"
source = {
    url = "git://github.com/starius/luahs.git",
}
description = {
    summary = "Lua bindings to hyperscan",
    homepage = "https://github.com/starius/luahs",
    license = "BSD 3-clause",
    detailed = [[
Lua bindings to hyperscan.

Hyperscan is high-performanceregular expression matching library.
]],
}
dependencies = {
    "lua >= 5.1",
}
external_dependencies = {
    HS = {
        header = "hs/hs.h",
        library = "hs",
    },
}

local platforms_modules = {
    ['luahs'] = {
        libraries = {"hs", "stdc++", "m"},
    },
}

build = {
    type = "builtin",
    modules = {
        ['luahs'] = {
            sources = {
                "src/luahs/constants.c",
                "src/luahs/util.c",
                "src/luahs/compile.c",
                "src/luahs/database.c",
                "src/luahs/scratch.c",
                "src/luahs/scan.c",
                "src/luahs/luahs.c",
            },
            incdirs = {"$(HS_INCDIR)"},
            libdirs = {"$(HS_LIBDIR)"},
            libraries = {"hs"},
        },
    },
    platforms = {
        unix = {
            modules = platforms_modules,
        },
        mingw32 = {
            modules = platforms_modules,
        },
    },
}
