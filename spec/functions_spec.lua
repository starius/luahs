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

describe("compilation", function()

    it("compiles simple pattern", function()
        local db = luahs.functions.compile {
            expression = 'aaa',
            mode = luahs.constants.compile_mode.HS_MODE_BLOCK,
        }
    end)

    it("specify platform", function()
        local db = luahs.functions.compile {
            expression = 'aaa',
            mode = luahs.constants.compile_mode.HS_MODE_BLOCK,
            platform = luahs.functions.populate_platform(),
        }
    end)

    it("throws on bad arguments", function()
        assert.has_error(function()
            luahs.functions.compile()
        end)
        assert.has_error(function()
            luahs.functions.compile({})
        end)
        assert.has_error(function()
            luahs.functions.compile({expression='aaa'})
        end)
        assert.has_error(function()
            luahs.functions.compile({expression='aaa', mode=42})
        end)
        assert.has_error(function()
            luahs.functions.compile {
                expression = 'aaa',
                mode = luahs.constants.compile_mode.HS_MODE_BLOCK,
                platform = 'foo',
            }
        end)
    end)

end)
