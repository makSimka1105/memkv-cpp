#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "commands/script/helpers.hpp"
#include "commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "schema/combinators/field.hpp"
#include "schema/parsers/arg.hpp"
#include "schema/parsers/variadic.hpp"
#include "schema/schema.hpp"

namespace valkey::commands {

struct FuncCall {
    std::string name;
    int64_t numkeys;
    std::vector<std::string> rest;
};

using FuncCallSchema = schema::Schema<FuncCall,
    schema::Field<&FuncCall::name, schema::Arg<std::string>>,
    schema::Field<&FuncCall::numkeys, schema::Arg<int64_t>>,
    schema::Field<&FuncCall::rest, schema::Variadic<std::string>>>;

io::Reply ExecFuncCall(engine::Engine& eng, const FuncCall& cmd) {
    const engine::ScriptFn* fn = eng.FindFunction(cmd.name);
    if (fn == nullptr) {
        return io::MakeErr("ERR no such function '" + cmd.name + "'");
    }
    auto split = SplitKeysArgv(cmd.rest, cmd.numkeys);
    if (!split) {
        return std::move(split).error();
    }
    return (*fn)(split->keys, split->argv);
}

using FuncCallTrait = CommandTrait<"FUNCCALL", FuncCallSchema, &ExecFuncCall>;

}  // namespace valkey::commands
