# memkv

![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.23+-064F8C?logo=cmake&logoColor=white)
![Commands](https://img.shields.io/badge/commands-58-DC382D?logo=redis&logoColor=white)

In-memory key-value database in modern C++ that speaks a subset of the
[Valkey](https://valkey.io/) / Redis command language. It supports five data types, transactions,
key expiry, a memory limit with OOM protection, and its own scripting language with a Pratt
parser and a tree-walking interpreter. Around 10k lines, no runtime dependencies.

```
> ZADD board 2 b 1 a 3 c
(integer) 3
> ZRANGE board 0 -1 WITHSCORES
1) "a"
2) "1"
3) "b"
4) "2"
5) "c"
6) "3"
> MULTI
OK
> SET foo bar
QUEUED
> GET foo
QUEUED
> EXEC
1) OK
2) "bar"
```

## Features

| Area | Commands |
|---|---|
| String | `SET` `GET` `APPEND` `STRLEN` `EXPIRE` `TTL` |
| List | `LPUSH` `RPUSH` `LPOP` `RPOP` `LLEN` `LRANGE` `LINDEX` `LSET` `LINSERT` |
| Set | `SADD` `SREM` `SISMEMBER` `SMEMBERS` `SCARD` `SUNION` `SINTER` `SDIFF` `SMOVE` |
| Sorted set | `ZADD` `ZINCRBY` `ZSCORE` `ZRANK` `ZREVRANK` `ZRANGE` `ZREVRANGE` `ZRANGEBYSCORE` `ZCOUNT` `ZCARD` `ZREM` `ZPOPMIN` `ZPOPMAX` |
| Geo | `GEOADD` `GEOPOS` `GEODIST` `GEOSEARCH` `GEOSEARCHSTORE` |
| Keys | `DEL` `EXISTS` `KEYS pattern` `TYPE` `DBSIZE` `FLUSHDB` `MEMORY USAGE` |
| Server | `CONFIG GET/SET maxmemory`, `--maxmemory 64mb` on startup |
| Transactions | `MULTI` `EXEC` `DISCARD` |
| Scripting | `EVAL` `FUNC` `FUNCCALL` |

- **Sorted sets on a skip list** with per-level spans, so rank queries (`ZRANK`, `ZRANGE`) run in
  O(log n) instead of a linear walk.
- **Geo search** with the haversine formula; `GEOSEARCH` supports `FROMLONLAT` / `FROMMEMBER`,
  `BYRADIUS` / `BYBOX`, `ASC` / `DESC`, `COUNT`, and options in any order.
- **Memory accounting.** Every stored type reports its size; writes that would exceed `maxmemory`
  fail with `OOM command not allowed when used memory > 'maxmemory'`.
- **Lazy expiry.** Expired keys are dropped on access and are excluded from `EXISTS`, `DBSIZE` and
  `KEYS`. `KEYS` uses an iterative glob matcher, so patterns like `*a*a*a*b` cannot blow up the stack.

## Scripting

Scripts run inside the engine and can call any command through `call(...)`. `FUNC` registers a
named function, `FUNCCALL name numkeys key... arg...` runs it:

```
> FUNC tagall "for tag in argv { call(\"SADD\", keys[0], tag); } return call(\"SCARD\", keys[0]);"
OK
> FUNCCALL tagall 1 tags red green red blue
(integer) 3
```

The language has `let`, `if`/`else`, `while`, `for ... in`, `return`, `and`/`or`/`not`, lists,
integers, floats, strings, booleans and `nil`, plus builtins `call`, `len` and `type`. Expressions are parsed with a Pratt
parser driven by binding-power tables, and the interpreter enforces an iteration budget so a
runaway loop cannot freeze the server.

## Architecture

The core idea: **a command is declared once, as data.** Each command is a plain struct, a schema
describing its arguments, and an exec function:

```cpp
struct ZRange {
    std::string key;
    int64_t start;
    int64_t stop;
    bool withscores;
};

using ZRangeSchema = schema::Schema<ZRange,
    schema::Field<&ZRange::key, schema::Arg<std::string>>,
    schema::Field<&ZRange::start, schema::Arg<int64_t>>,
    schema::Field<&ZRange::stop, schema::Arg<int64_t>>,
    schema::Field<&ZRange::withscores, schema::Flag<"WITHSCORES">>>;

using ZRangeTrait = CommandTrait<"ZRANGE", ZRangeSchema, &ExecZRange>;
```

From that, templates generate the parser, argument-count checks and error messages at compile
time. Adding a command is one new header.

- **Schema combinators** (`Seq`, `Opt`, `OneOf`, `AnyOrder`, `Repeat`, `Group`) and parsers
  (`Arg<T>`, `Flag<"...">`, `Keyword`, `Enum`, `Variadic`) are constrained by concepts and compose
  like parser combinators. `AnyOrder` accepts options in any order in two passes with fold
  expressions and early exit.
- **Dispatch** is a `std::variant` of all command structs plus an overload set generated from a
  type list of traits, called through `std::visit`. No virtual calls and no string switch at runtime.
- **Lexer** built from rule types (`Word`, `Int`, `Float`, `QuotedString` with escapes) working on
  `std::string_view` without allocations.
- **Error handling** through `std::expected` with an assign-or-return helper instead of exceptions.

```
lib/
  command/    lexer, token cursor, schema combinators, one header per command
  engine/     Engine, keyspace, memory tracker, dispatcher, sessions with MULTI state
  structures/ skip list, sorted set, geo index
  types/      stored types and their size and emptiness traits
  script/     lexer, Pratt parser, AST, evaluator, builtins
  meta/       type lists, fixed_string, overload, table helpers
  io/         reply model and console formatting
bin/          REPL over stdin / stdout
tests/        GoogleTest suites per data type, transactions and scripting
```

## Build

Requirements: GCC 14+ or Clang 18+ with C++23, CMake 3.23+.

```bash
cmake -S . -B build
cmake --build build -j
./build/bin/valkey --maxmemory 64mb
```

Tests (GoogleTest is fetched by CMake):

```bash
cmake -S . -B build -DVALKEY_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

`scripts/check.sh` builds and runs the tests with GCC and Clang, with and without
AddressSanitizer and UndefinedBehaviorSanitizer.
