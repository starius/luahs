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
