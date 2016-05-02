-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2015 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

describe("constants", function()

    it("has constants", function()
        assert.equal(0, luahs.constants.errors.HS_SUCCESS)
        assert.equal(2, luahs.constants.pattern_flags.HS_FLAG_DOTALL)
        assert.equal(1, luahs.constants.compile_mode.HS_MODE_BLOCK)
        assert.equal(4, luahs.constants.cpu_features.HS_CPU_FEATURES_AVX2)
        assert.equal(2, luahs.constants.cpu_tuning.HS_TUNE_FAMILY_IVB)
    end)

end)
