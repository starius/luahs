-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

describe("compilation", function()

    it("compiles simple pattern", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
        }
    end)

    it("current platform", function()
        local platform = luahs.currentPlatform()
        assert.truthy(platform.tune)
        assert.truthy(platform.cpu_features)
    end)

    it("specify platform", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
            platform = {
                tune = luahs.cpu_tuning.HS_TUNE_FAMILY_GENERIC,
            },
        }
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
            platform = {
                tune = {
                    luahs.cpu_tuning.HS_TUNE_FAMILY_GENERIC,
                    luahs.cpu_tuning.HS_TUNE_FAMILY_SNB,
                },
            },
        }
        local db = luahs.compile {
            expression = 'aaa',
            mode = luahs.compile_mode.HS_MODE_BLOCK,
            platform = luahs.currentPlatform(),
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

    it("specify multiple modes", function()
        local db = luahs.compile {
            expression = 'aaa',
            mode = {
                luahs.compile_mode.HS_MODE_STREAM,
                luahs.compile_mode.HS_MODE_SOM_HORIZON_LARGE,
            },
            flags = HS_FLAG_SOM_LEFTMOST,
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
        local info = luahs.expressionInfo('aaa?')
        assert.equal(2, info.min_width)
        assert.equal(3, info.max_width)
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
