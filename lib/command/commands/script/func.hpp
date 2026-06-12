#pragma once

#include <string>

#include "command/commands/script/helpers.hpp"
#include "command/commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/parsers/arg.hpp"
#include "command/schema/schema.hpp"
#include "script/lexer/lexer.hpp"
#include "script/parser/parser.hpp"
#include "script/runtime.hpp"

namespace valkey::commands {

struct Func {
    std::string name;
    std::string body;
};

using FuncSchema = schema::Schema<Func,
    schema::Field<&Func::name, schema::Arg<std::string>>,
    schema::Field<&Func::body, schema::Arg<std::string>>>;

io::Reply ExecFunc(engine::Engine& eng, const Func& cmd) {
    auto tokens = script::Tokenize(cmd.body);
    if (!tokens) {
        return io::MakeErr("ERR " + tokens.error().message);
    }

    script::Parser parser(*tokens);
    auto prog = parser.ParseProgram();
    if (!prog) {
        return io::MakeErr("ERR " + prog.error().message);
    }

    eng.RegisterFunction(cmd.name, script::Compile(eng, std::move(*prog)));
    return io::Ok();
}

using FuncTrait = CommandTrait<"FUNC", FuncSchema, &ExecFunc>;

}  // namespace valkey::commands
