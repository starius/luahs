-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2015 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

describe("database", function()

    it("get description of the database as string", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local info = db:info()
        local as_string = tostring(db)
        assert.equal(info, as_string)
        assert.truthy(as_string:match('Version'))
    end)

    it("serializes and deserializes database", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local data = db:serialize()
        assert.truthy(#data > 0)
        local db1 = luahs.deserialize(data)
        assert.equal(db:info(), db1:info())
        assert.equal(db:serialize(), db1:serialize())
    end)

end)
