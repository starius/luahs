-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
-- See the LICENSE file for terms of use.

local luahs = require 'luahs'

local pattern = arg[1]
local filename = arg[2]
local stream_mode = arg[3] == '--stream'

if not pattern then
    print(('Usage: %s <pattern> <input file> [--stream]'):format(arg[0]))
    os.exit(255)
end

local function getMode()
    if stream_mode then
        return luahs.compile_mode.HS_MODE_STREAM
    else
        return luahs.compile_mode.HS_MODE_BLOCK
    end
end

local function printHits(hits)
    for _, hit in ipairs(hits) do
        print(('Match for pattern %q at offset %d'):format(pattern, hit.to))
    end
end

local db = luahs.compile {
    expression = pattern,
    flags = luahs.pattern_flags.HS_FLAG_DOTALL,
    mode = getMode()
}
local scratch = db:makeScratch()

local file = io.open(filename, 'r')

if stream_mode then
    local stream = db:makeStream()
    local file_size = file:seek("end", 0)
    file:seek("set", 0)
    print(('Scanning %d bytes with Hyperscan'):format(file_size))
    while true do
        local chunk = file:read(8192)
        if not chunk then
            break
        end
        local hits = stream:scan(chunk, scratch)
        printHits(hits)
    end
else
    local content = file:read('*a')
    file:close()
    print(('Scanning %d bytes with Hyperscan'):format(#content))
    local hits = db:scan(content, scratch)
    printHits(hits)
end
