-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
-- See the LICENSE file for terms of use.

os.exit = function() end
require "busted.runner"({ standalone = false, batch = true })
