-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2015 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

describe("functions", function()

    it("current_platform", function()
        local platform = luahs.current_platform()
        assert.truthy(platform.tune)
        assert.truthy(platform.cpu_features)
    end)

    it("version", function()
        local version = luahs.version()
        assert.truthy(version:match('%.'))
    end)

end)

describe("compilation", function()

    it("compiles simple pattern", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
    end)

    it("specify platform", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
            platform = {
                tuning = luahs.cpu_tuning.HS_TUNE_FAMILY_GENERIC,
            },
        }
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
            platform = {
                tuning = {
                    luahs.cpu_tuning.HS_TUNE_FAMILY_GENERIC,
                    luahs.cpu_tuning.HS_TUNE_FAMILY_SNB,
                },
            },
        }
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
            platform = luahs.current_platform(),
        }
    end)

    it("specify flags", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
            flags = luahs.pattern_flags.HS_FLAG_CASELESS,
        }
    end)

    it("specify flags as list", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
            flags = {
                luahs.pattern_flags.HS_FLAG_CASELESS,
                luahs.pattern_flags.HS_FLAG_DOTALL,
            },
        }
    end)

    it("compiles multiple patterns", function()
        local db = luahs.compile {
            expressions = {
                'aaa',
                'bbb',
            },
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
    end)

    it("compiles multiple patterns with parameters", function()
        local db = luahs.compile {
            expressions = {
                'aaa',
                {
                    expression = 'bbb',
                    id = 42,
                    flags = luahs.pattern_flags.HS_FLAG_CASELESS,
                },
            },
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
    end)

    it("compiles multiple patterns with multiple flags", function()
        local db = luahs.compile {
            expressions = {
                'aaa',
                {
                    expression = 'bbb',
                    id = 42,
                    flags = {
                        luahs.pattern_flags.HS_FLAG_CASELESS,
                        luahs.pattern_flags.HS_FLAG_DOTALL,
                    },
                },
            },
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
    end)

    it("compiles multiple patterns with extended flags", function()
        local db = luahs.compile {
            expressions = {
                'aaa',
                {
                    expression = 'bbb',
                    min_offset = 100,
                    max_offset = 140,
                    min_length = 1,
                },
            },
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
    end)

    it("get information about RE", function()
        local db = luahs.expression_info('aaa?')
        assert.equal(2, db.min_width)
        assert.equal(3, db.max_width)
    end)

    it("throws on bad arguments", function()
        assert.has_error(function()
            luahs.compile()
        end)
        assert.has_error(function()
            luahs.compile({})
        end)
        assert.has_error(function()
            luahs.compile({expression='aaa'})
        end)
        assert.has_error(function()
            luahs.compile({expression='aaa', mode=42})
        end)
        assert.has_error(function()
            luahs.compile {
                expression = 'aaa',
                mode = luahs.compile_mode.HS_MODE_BLOCK,
                platform = 'foo',
            }
        end)
        assert.has_error(function()
            luahs.compile {
                expressions = 'aaa',
                mode = luahs.compile_mode.HS_MODE_BLOCK,
            }
        end)
        assert.has_error(function()
            luahs.compile {
                expression = {
                    'aaa',
                    'bbb',
                },
                mode = luahs.compile_mode.HS_MODE_BLOCK,
            }
        end)
        assert.has_error(function()
            luahs.compile {
                expressions = {
                    {},
                },
                mode = luahs.compile_mode.HS_MODE_BLOCK,
            }
        end)
        assert.has_error(function()
            luahs.compile {
                expressions = {
                    {
                        expression = 'aaa',
                        flags = 10000001,
                    },
                },
                mode = luahs.compile_mode.HS_MODE_BLOCK,
            }
        end)
        assert.has_error(function()
            luahs.compile {
                expressions = {
                    {
                        expression = 'aaa',
                        id = '',
                    },
                },
                mode = luahs.compile_mode.HS_MODE_BLOCK,
            }
        end)
        assert.has_error(function()
            luahs.compile {
                expressions = {
                    {
                        expression = 'aaa',
                        min_length = '',
                    },
                },
                mode = luahs.compile_mode.HS_MODE_BLOCK,
            }
        end)
        assert.has_error(function()
            luahs.compile {
                expressions = {
                    {
                        expression = 'aaa',
                        min_length = 4,
                    },
                },
                mode = luahs.compile_mode.HS_MODE_BLOCK,
            }
        end)
    end)

end)

describe("database.info", function()

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

end)

describe("serialization", function()

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

end)
