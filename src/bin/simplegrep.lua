-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

local pattern = arg[1]
local filename = arg[2]

if not pattern then
    print(('Usage: %s <pattern> <input file>'):format(arg[0]))
    os.exit(255)
end

local db = luahs.compile {
    expression = pattern,
    mode = luahs.compile_mode.HS_MODE_BLOCK,
}
local scratch = db:makeScratch()

local file = io.open(filename, 'r')
local content = file:read('*a')
file:close()

print(('Scanning %d bytes with Hyperscan'):format(#content))

local hits = db:scan(content, scratch)
for _, hit in ipairs(hits) do
    print(('Match for pattern %q at offset %d'):format(pattern, hit.to))
end
