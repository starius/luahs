-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

describe("scratch", function()

    local db = luahs.compile {
        expression = 'aaa',
        mode = luahs.compile_mode.HS_MODE_BLOCK,
    }

    local db1 = luahs.compile {
        expressions = {
            'aaa',
            'bbb',
            'foo.{10,20}bar',
        },
        mode = luahs.compile_mode.HS_MODE_BLOCK,
    }

    it("creates scratch", function()
        local scratch = db:makeScratch()
    end)

    it("get size of scratch", function()
        local scratch = db:makeScratch()
        local size = scratch:size()
        assert.truthy(size > 0)
    end)

    it("grow scratch", function()
        local scratch = db:makeScratch()
        local size = scratch:size()
        assert.truthy(size > 0)
        scratch:grow(db1)
        local size1 = scratch:size()
        assert.truthy(size1 > size)
        local scratch1 = db1:makeScratch()
        assert.equal(scratch:size(), scratch1:size())
    end)

end)
