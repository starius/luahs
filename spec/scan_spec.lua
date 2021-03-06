-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

describe("scan", function()

    it("scans simple expression", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local scratch = db:makeScratch()
        assert.same({
            {id=0, from=0, to=3},
        }, db:scan('aaa', scratch))
        assert.same({}, db:scan('bbb', scratch))
        assert.same({
            {id=0, from=0, to=3},
            {id=0, from=0, to=4},
        }, db:scan('aaaa', scratch))
    end)

    it("throws if text is of bad type", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local scratch = db:makeScratch()
        assert.has_error(function()
            db:scan(123, scratch)
        end)
    end)

    it("scans multiple expressions", function()
        local db = luahs.compile {
            expressions = {
                'aaa',
                {expression='bbb', id=1},
            },
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local scratch = db:makeScratch()
        assert.same({
            {id=0, from=0, to=3},
        }, db:scan('aaa', scratch))
        assert.same({
            {id=1, from=0, to=3},
        }, db:scan('bbb', scratch))
    end)

    it("scans expression with SOM", function()
        local db = luahs.compile {
            expression = 'foo.{1,20}bar',
            flags = luahs.pattern_flags.HS_FLAG_SOM_LEFTMOST,
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local scratch = db:makeScratch()
        assert.same({
            {id=0, from=1, to=8},
            {id=0, from=1, to=12},
        }, db:scan(' foo bar bar', scratch))
    end)

    it("throws if mode doesn't match", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_VECTORED,
        }
        local scratch = db:makeScratch()
        assert.has_error(function()
            db:scan('aaa', scratch)
        end)
        assert.has_error(function()
            db:makeStream()
        end)
        --
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local scratch = db:makeScratch()
        assert.has_error(function()
            db:scan({'aaa'}, scratch)
        end)
        assert.has_error(function()
            db:makeStream()
        end)
        --
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_STREAM,
        }
        local scratch = db:makeScratch()
        assert.has_error(function()
            db:scan('aaa', scratch)
        end)
        assert.has_error(function()
            db:scan({'aaa'}, scratch)
        end)
    end)

    it("scans in vectored mode", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_VECTORED,
        }
        local scratch = db:makeScratch()
        assert.same({
            {id=0, from=0, to=3},
        }, db:scan({'aa', 'a'}, scratch))
        assert.same({}, db:scan({'bbb'}, scratch))
        assert.same({
            {id=0, from=0, to=3},
            {id=0, from=0, to=4},
        }, db:scan({'', 'a', '', 'aaa', ''}, scratch))
    end)

    it("scans unicode text (UTF-8)", function()
        local db = luahs.compile {
            expression = '1.3',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
            flags = luahs.pattern_flags.HS_FLAG_UTF8,
        }
        local scratch = db:makeScratch()
        assert.same({
            {id=0, from=0, to=4},
        }, db:scan('1ф3', scratch))
        -- control
        local db = luahs.compile {
            expression = '1.3',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
        local scratch = db:makeScratch()
        assert.same({}, db:scan('1ф3', scratch))
    end)

end)
