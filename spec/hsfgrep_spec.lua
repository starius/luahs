-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

describe("hsfgrep.lua", function()

    it("look for multiple expressions in multiple files",
    function()
        local expected = [[
LICENSE	Copyright	9
LICENSE	20.6	18
src/luahs/luahs.h	Copyright	48
src/luahs/luahs.h	20.6	57
]]
        local cmd = table.concat({
            'lua', 'src/bin/hsfgrep.lua',
            '-p', 'Copyright', '20.6',
            '-i', 'LICENSE', 'src/luahs/luahs.h',
        }, ' ')
        local f = io.popen(cmd, 'r')
        local observed = f:read('*a')
        f:close()
        assert.equal(expected, observed)
    end)

end)
