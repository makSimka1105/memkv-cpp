#pragma once

#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "engine/engine.hpp"
#include "engine/script_registry.hpp"
#include "io/reply.hpp"
#include "script/ast.hpp"
#include "script/evaluator/evaluator.hpp"
#include "script/lexer/scanner.hpp"
#include "script/parser/parser.hpp"

namespace valkey::script {

io::Reply Run(engine::Engine& engine, std::string_view src,
                     std::span<const std::string> keys, std::span<const std::string> argv) {
    auto tokens = Lex(src);
    if (!tokens) {
        return io::MakeErr("ERR " + tokens.error().message);
    }
    Parser parser(*tokens);
    auto program = parser.ParseProgram();
    if (!program) {
        return io::MakeErr("ERR " + program.error().message);
    }
    return Execute(engine, *program, keys, argv);
}

engine::ScriptFn Compile(engine::Engine& engine, Program program) {
    return [program = std::move(program), &engine](
               std::span<const std::string> keys,
               std::span<const std::string> argv) -> io::Reply {
        return Execute(engine, program, keys, argv);
    };
}

}  // namespace valkey::script
