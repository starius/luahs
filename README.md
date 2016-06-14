# luahs

[![Build Status][build-status]][travis]
[![Coverage Status][coveralls-badge]][coveralls-page]
[![License][license]](LICENSE)

Lua bindings to hyperscan, high-performance regular expression matching library

[travis]: https://travis-ci.org/starius/luahs
[build-status]: https://travis-ci.org/starius/luahs.png?branch=master
[coveralls-page]: https://coveralls.io/r/starius/luahs
[coveralls-badge]: https://coveralls.io/repos/starius/luahs/badge.png
[license]: https://img.shields.io/badge/License-BSD3-brightgreen.png

## Installation

You need hyperscan and luarocks installed.

```
$ luarocks make
```

You can provide `HS_DIR` if you have installed hyperscan
to unusual place.

```
$ luarocks make HS_DIR=/usr/local
```

## Reference

This rock has two modules:

  * `luahs` has all functions including compilation of patterns
  * `luahs_runtime` has all functions except
    `compile`, `expressionInfo` and `currentPlatform`.

`luahs` is linked against `libhs` and
`luahs_runtime` is linked against `libhs_runtime`.

Require `luahs` module from Lua:

```lua
luahs = require 'luahs`
```

You can find unit tests in directory `spec`.

### Constants

All constants used by hyperscan are available in sub-tables of `luahs`:

  * `luahs.errors` -- [error codes][errors]
  * `luahs.compile_mode` -- [compilation mode][compile_mode] (block, stream, etc)
  * `luahs.pattern_flags` -- [pattern flags][pattern_flags] (case-insensitive, etc)
  * `luahs.extended_parameters` -- [extended parameters of pattern][extended_parameters]
  * `luahs.cpu_features` -- [CPU feature support flags][cpu_features]
  * `luahs.cpu_tuning` -- [CPU tuning flags][cpu_tuning]

[errors]: http://01org.github.io/hyperscan/dev-reference/api_constants.html#error-codes
[compile_mode]: http://01org.github.io/hyperscan/dev-reference/api_constants.html#compile-mode-flags
[pattern_flags]: http://01org.github.io/hyperscan/dev-reference/api_constants.html#pattern-flags
[extended_parameters]: http://01org.github.io/hyperscan/dev-reference/api_constants.html#hs-expr-ext-flags
[cpu_features]: http://01org.github.io/hyperscan/dev-reference/api_constants.html#cpu-feature-support-flags
[cpu_tuning]: http://01org.github.io/hyperscan/dev-reference/api_constants.html#cpu-tuning-flags

Example:

```lua
> print(luahs.errors.HS_SUCCESS)
0
```

### Compilation of patterns

Compilation is done with function `luahs.compile`.
It takes a regular expression (or several regular expressions)
and parameters of compilation and returns a database.

```lua
db = luahs.compile {
    expression = 'aaa',
    mode = luahs.compile_mode.HS_MODE_BLOCK,
}
```

Provide pattern flags:

```lua
db = luahs.compile {
    expression = 'aaa',
    mode = luahs.compile_mode.HS_MODE_BLOCK,
    flags = luahs.pattern_flags.HS_FLAG_CASELESS,
}
```

Provide multiple flags:

```lua
db = luahs.compile {
    expression = 'aaa',
    mode = luahs.compile_mode.HS_MODE_BLOCK,
    flags = {
        luahs.pattern_flags.HS_FLAG_CASELESS,
        luahs.pattern_flags.HS_FLAG_DOTALL,
    },
}
```

`mode` can also be a list in case of Start-Of-Match (SOM):

```lua
db = luahs.compile {
    expression = 'aaa',
    mode = {
        luahs.compile_mode.HS_MODE_STREAM,
        luahs.compile_mode.HS_MODE_SOM_HORIZON_LARGE,
    },
    flags = HS_FLAG_SOM_LEFTMOST,
}
```

Compile multiple patterns:

```lua
db = luahs.compile {
    expressions = {
        'aaa',
        'bbb',
    },
    mode = luahs.compile_mode.HS_MODE_BLOCK,
}
```

If you compile multiple patterns and you need provide
flags, identifiers or extended parameters of a pattern,
you should provide a table with the following fields
as a pattern:

  * (required) `expression` - pattern itself
  * (optional) `flags` - flags, integer of list of integers
  * (optional) `id` - identifier of a pattern, defaults to 0
  * (optional) `min_offset` - the minimum end offset in the data stream at
    which this expression should match successfully
  * (optional) `max_offset` - the maximum end offset in the data stream at
    which this expression should match successfully
  * (optional) `min_length` - minimum match length (from start to end)
    required to successfully match this expression

Example:

```lua
db = luahs.compile {
    expressions = {
        {
            expression = 'aaa',
            id = 1,
            flags = luahs.pattern_flags.HS_FLAG_CASELESS,
            min_offset = 100,
            max_offset = 140,
        },
        {
            expression = 'b.{1,20}b.{1,20}b',
            id = 2,
            flags = {
                luahs.pattern_flags.HS_FLAG_CASELESS,
                luahs.pattern_flags.HS_FLAG_DOTALL,
            },
            min_offset = 200,
            max_offset = 800,
            min_length = 20,
        },
    },
    mode = luahs.compile_mode.HS_MODE_BLOCK,
}
```

#### Platform

You can provide a platform on which database runs:

```lua
db = luahs.compile {
    expression = 'aaa',
    mode = luahs.compile_mode.HS_MODE_BLOCK,
    platform = {
        tune = luahs.cpu_tuning.HS_TUNE_FAMILY_GENERIC,
    }
}
```

`platform` table has the following fields, all are optional:

  * `cpu_features` - CPU feature support flags
  * `tune` - CPU tuning flags

Value can be an integer or a list of integers.

Function `luahs.currentPlatform()` returns such a table for
current platform.

#### expressionInfo

Function `luahs.expressionInfo` returns information about
the expression instead of database:

```lua
> info = luahs.expressionInfo('a?a?a?b')
> print(info.min_width)
1
> print(info.max_width)
4
```

Optionally, pattern flags can be provided as an integer or as a table:

```lua
info = luahs.expressionInfo(
    'a?a?a?',
    luahs.pattern_flags.HS_FLAG_ALLOWEMPTY
)
info = luahs.expressionInfo(
    'a?a?a?',
    {
        luahs.pattern_flags.HS_FLAG_ALLOWEMPTY,
        luahs.pattern_flags.HS_FLAG_CASELESS,
    }
)
```

See [fields of table `info`][hs_expr_info].

[hs_expr_info]: http://01org.github.io/hyperscan/dev-reference/api_files.html#c.hs_expr_info

### Scan a database against a text

To scan a text against a database, you need a scratch object.
It can be created using method `db:makeScratch()`:

```lua
db = luahs.compile {
    expression = 'aaa',
    mode = luahs.compile_mode.HS_MODE_BLOCK,
}
scratch = db:makeScratch()
```

Then you can scan a text:

```lua
hits = db:scan('aaa')
-- hits is {{id=0, from=0, to=3}}
```

Method `scan` returns hits. Each hit is a table with
the following fields:

  * `id` - identifier of a pattern
  * `from` - start of a hit, 0-based.
    If Start-Of-Match flag is not set, `from` is always equal to 0.
  * `to` - end of a hit, 0-based index of a first byte after a hit.

If a database is compiled in vectored mode
(`mode` = `luahs.compile_mode.HS_MODE_VECTORED`), you have to pass
a table of strings to `scan` method:

```lua
hits = db:scan({'a', 'aa'})
```

#### Stream mode

To scan in stream mode, you need a stream object:

```lua
db = luahs.compile {
    expression = 'abc',
    mode = luahs.compile_mode.HS_MODE_STREAM,
}
scratch = db:makeScratch()
stream = db:makeStream()
```

Apply method `scan` to the stream object:

```lua
hits1 = stream:scan('a', scratch) -- hits1 is {}
hits2 = stream:scan('b', scratch) -- hits2 is {}
hits3 = stream:scan('c', scratch) -- hits3 is {{id=0, from=0, to=3}}
```

You have to close a stream using method `close`:

```
hits = stream:close(scratch)
```

Method `close` also can return some hits.

You can reset a stream object:

```lua
hits = stream:reset(scratch)
```

A call to `reset` has the same effect to a call to `close` followed by
creating a new stream for the same database.

Stream objects of the same database can be assigned to each other:

```lua
db = luahs.compile {
    expression = 'aaa$',
    mode = luahs.compile_mode.HS_MODE_STREAM,
}
scratch = db:makeScratch()
stream1 = db:makeStream()
stream2 = db:makeStream()
stream1:scan('a', scratch) -- returns {}
stream1:scan('a', scratch) -- returns {}
stream1:scan('a', scratch) -- returns {}
stream2:assign(stream1, scratch) -- returns {}
-- stream2 := stream1;
stream2:reset(scratch) -- returns {{id=0, from=0, to=3}}
```

You can also clone a stream: `clone = stream:clone()`.

You can get a database back from a stream using
method `stream:database()`.

### Database serialization

TODO

### Utility functions

TODO
