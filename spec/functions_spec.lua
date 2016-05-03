-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2015 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

describe("functions", function()

    it("populate_platform", function()
        local platform = luahs.functions.populate_platform()
        assert.truthy(platform.tune)
        assert.truthy(platform.cpu_features)
    end)

end)
