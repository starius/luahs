-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2015 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

describe("util", function()

    it("version", function()
        local version = luahs.version()
        assert.truthy(version:match('%.'))
    end)

end)
