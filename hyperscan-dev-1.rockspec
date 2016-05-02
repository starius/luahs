-- lua-hyperscan, Lua bindings to hyperscan
-- Copyright (C) 2015 Boris Nagaev
-- See the LICENSE file for terms of use.

package = "hyperscan"
version = "dev-1"
source = {
    url = "git://github.com/starius/lua-hyperscan.git",
}
description = {
    summary = "Lua bindings to hyperscan",
    homepage = "https://github.com/starius/lua-hyperscan",
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
        header = "hs/hs.h"
    },
}
build = {
    type = "builtin",
    modules = {
        ['hyperscan'] = {
            sources = {
                "src/hyperscan/constants.c",
                --"src/hyperscan/functions.c",
                "src/hyperscan/hyperscan.c",
            },
            incdirs = {"$(HS_INCDIR)"},
            libdirs = {"$(HS_LIBDIR)"},
            libraries = {"hs"},
        },
    },
    platforms = {
        unix = {
            modules = {
                ['hyperscan'] = {
                    libraries = {"stdc++", "m"},
                },
            },
        },
        mingw32 = {
            modules = {
                ['hyperscan'] = {
                    libraries = {"stdc++", "m"},
                },
            },
        },
    },
}
