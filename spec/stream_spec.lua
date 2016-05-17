-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

describe("stream", function()

    it("get description of the stream as string", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        local scratch = db:makeScratch()
        local stream = db:makeStream()
        assert.truthy(tostring(stream):match('^Hyperscan stream'))
    end)

    it("scans simple expression in stream mode", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        local scratch = db:makeScratch()
        local stream = db:makeStream()
        assert.same({}, stream:scan('a', scratch))
        assert.same({}, stream:scan('a', scratch))
        assert.same({{id=0, from=0, to=3}}, stream:scan('a', scratch))
        assert.same({}, stream:close(scratch))
    end)

    it("scans anchored expression in stream mode", function()
        local db = luahs.compile {
            expression = 'aaa$',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        local scratch = db:makeScratch()
        local stream = db:makeStream()
        assert.same({}, stream:scan('a', scratch))
        assert.same({}, stream:scan('a', scratch))
        assert.same({}, stream:scan('a', scratch))
        assert.same({{id=0, from=0, to=3}}, stream:close(scratch))
    end)

    it("resets stream", function()
        local db = luahs.compile {
            expression = 'aaa$',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        local scratch = db:makeScratch()
        local stream = db:makeStream()
        assert.same({}, stream:scan('a', scratch))
        assert.same({}, stream:scan('a', scratch))
        assert.same({}, stream:scan('a', scratch))
        assert.same({{id=0, from=0, to=3}}, stream:reset(scratch))
        assert.same({}, stream:scan('aaa', scratch))
        assert.same({{id=0, from=0, to=3}}, stream:close(scratch))
    end)

    it("assigns stream", function()
        local db = luahs.compile {
            expression = 'aaa$',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        local scratch = db:makeScratch()
        local stream1 = db:makeStream()
        local stream2 = db:makeStream()
        assert.same({}, stream1:scan('a', scratch))
        assert.same({}, stream1:scan('a', scratch))
        assert.same({}, stream1:scan('a', scratch))
        assert.same({}, stream2:assign(stream1, scratch))
        assert.same({{id=0, from=0, to=3}}, stream2:reset(scratch))
    end)

    it("throws if assign streams of different databases", function()
        local db1 = luahs.compile {
            expression = 'aaa$',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        local db2 = luahs.compile {
            expression = 'aaa$',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        local scratch = db1:makeScratch()
        local stream1 = db1:makeStream()
        local stream2 = db2:makeStream()
        assert.same({}, stream1:scan('a', scratch))
        assert.same({}, stream1:scan('a', scratch))
        assert.same({}, stream1:scan('a', scratch))
        assert.has_error(function()
            stream2:assign(stream1, scratch)
        end)
    end)

    it("clones stream", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        local stream = db:makeStream()
        local stream2 = stream:clone()
    end)

    it("closes stream", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        local stream = db:makeStream()
        local scratch = db:makeScratch()
        stream:close(scratch)
    end)

    it("get database", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        local stream = db:makeStream()
        assert.equal(db, stream:database())
        local scratch = db:makeScratch()
        stream:close(scratch)
        assert.equal(nil, stream:database())
    end)

    it("throws on using a closed stream", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        local stream = db:makeStream()
        local scratch = db:makeScratch()
        stream:close(scratch)
        assert.has_error(function()
            stream:scan('a', scratch)
        end)
        assert.has_error(function()
            stream:clone()
        end)
        assert.has_error(function()
            stream:close(scratch)
        end)
    end)

end)
