#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include "command/commands/script/helpers.hpp"
#include "command/commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "script/runtime.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/parsers/arg.hpp"
#include "command/schema/parsers/variadic.hpp"
#include "command/schema/schema.hpp"
namespace valkey::commands {

struct Eval {
    std::string body;
    int64_t numkeys;
    std::vector<std::string> rest;
};

using EvalSchema = schema::Schema<Eval,
    schema::Field<&Eval::body, schema::Arg<std::string>>,
    schema::Field<&Eval::numkeys, schema::Arg<int64_t>>,
    schema::Field<&Eval::rest, schema::Variadic<std::string>>>;

io::Reply ExecEval(engine::Engine& eng, const Eval& cmd) {
    auto split = SplitKeysArgv(cmd.rest, cmd.numkeys);
    if (!split) {
        return std::move(split).error();
    }
    return script::Run(eng, cmd.body, split->keys, split->argv);
}

using EvalTrait = CommandTrait<"EVAL", EvalSchema, &ExecEval>;

}  // namespace valkey::commands
