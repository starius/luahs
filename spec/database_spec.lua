-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
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
        assert.truthy(info:match('Version'))
        assert.truthy(as_string:match('^Hyperscan database'))
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

    it("returns size of database in bytes", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local data = db:serialize()
        assert.equal(db:size(), luahs.sizeOfDeserialized(data))
    end)

    it("returns size of stream in bytes", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        assert.truthy(db:streamSize() > 0)
    end)

    it("streamSize() throws if database is not in stream mode",
    function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        assert.has_error(function()
            db:streamSize()
        end)
    end)

    it("returns info of serialized database", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local data = db:serialize()
        assert.equal(db:info(), luahs.infoOfDeserialized(data))
    end)

    it("throws if deserializing garbage", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local data = db:serialize()
        assert.has_error(function()
            luahs.deserialize(data .. '\0')
        end)
        assert.has_error(function()
            luahs.sizeOfDeserialized(data .. '\0')
        end)
    end)

    -- https://github.com/01org/hyperscan/pull/22
    pending("sizeOfDeserialized throws if deserializing garbage",
    function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local data = db:serialize()
        assert.has_error(function()
            luahs.infoOfDeserialized(data .. '\0')
        end)
    end)

end)
