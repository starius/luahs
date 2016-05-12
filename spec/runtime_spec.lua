-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs_runtime = require 'luahs_runtime'
local luahs = require 'luahs'

describe("runtime", function()

    it("serializes and deserializes database", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local data = db:serialize()
        local db1 = luahs_runtime.deserialize(data)
        assert.equal(db:info(), db1:info())
        assert.equal(db:serialize(), db1:serialize())
    end)

end)
