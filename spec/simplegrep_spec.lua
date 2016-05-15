-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

describe("simplegrep.lua", function()

    it("look for expression in file", function()
        local expected = [[
Scanning 1475 bytes with Hyperscan
Match for pattern "dis" at offset 60
Match for pattern "dis" at offset 206
Match for pattern "dis" at offset 319
Match for pattern "dis" at offset 336
Match for pattern "dis" at offset 452
Match for pattern "dis" at offset 527
]]
        for _, stream in ipairs{"", "--stream"} do
            local cmd = 'lua src/bin/simplegrep.lua dis LICENSE ' .. stream
            local f = io.popen(cmd, 'r')
            local observed = f:read('*a')
            f:close()
            assert.equal(expected, observed)
        end
    end)

end)
