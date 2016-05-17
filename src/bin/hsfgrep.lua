-- luahs, Lua bindings to hyperscan
-- Copyright (C) 2016 Boris Nagaev
-- See the LICENSE file for terms of use.

local argparse = require 'argparse'
local luahs = require 'luahs'

local parser = argparse('hsfgrep', 'Match multiple patterns with Hyperscan')
parser:option('-p --patterns', 'Patterns'):args('+'):count(1)
parser:option('-i --input', 'Input files'):args('+'):count(1)
local args = parser:parse()

local expressions = {}
for i, pattern in ipairs(args.patterns) do
    expressions[i] = {
        expression = pattern,
        id = i,
    }
end
local db = luahs.compile {
    expressions = expressions,
    flags = luahs.pattern_flags.HS_FLAG_DOTALL,
    mode = luahs.compile_mode.HS_MODE_STREAM,
}
local scratch = db:makeScratch()
local stream = db:makeStream()

for _, filename in ipairs(args.input) do

    local function printHits(hits)
        local fmt = '%s\t%s\t%d'
        for _, hit in ipairs(hits) do
            print(fmt:format(filename, args.patterns[hit.id], hit.to))
        end
    end

    local file = io.open(filename, 'r')
    while true do
        local chunk = file:read(8192)
        if not chunk then
            break
        end
        local hits = stream:scan(chunk, scratch)
        printHits(hits)
    end
    local hits = stream:reset(scratch)
    printHits(hits)
end
