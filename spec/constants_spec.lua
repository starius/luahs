-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

describe("constants", function()

    it("has constants", function()
        assert.equal(0, luahs.errors.HS_SUCCESS)
        assert.equal(2, luahs.pattern_flags.HS_FLAG_DOTALL)
        assert.equal(1, luahs.compile_mode.HS_MODE_BLOCK)
        assert.equal(4, luahs.cpu_features.HS_CPU_FEATURES_AVX2)
        assert.equal(2, luahs.cpu_tuning.HS_TUNE_FAMILY_IVB)
        assert.equal(-1, luahs.HS_OFFSET_PAST_HORIZON)
        assert.truthy(luahs.UINT_MAX > 0)
    end)

    it("decodes constants", function()
        assert.equal("string", type(luahs.decodeError(0)))
    end)

    it("returns 'Unknown error.' on unknown error", function()
        assert.truthy(luahs.decodeError(42):match('[Uu]nknown'))
    end)

end)
