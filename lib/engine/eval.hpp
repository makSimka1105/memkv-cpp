#pragma once

#include <string>
#include <variant>
#include <vector>

#include "commands/all.hpp"
#include "commands/tree.hpp"
#include "engine/dispatcher.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "lexer/lexer.hpp"
#include "lexer/types.hpp"
#include "parser/cmd.hpp"
#include "parser/parser.hpp"

namespace valkey::engine {

io::Reply Eval(engine::Engine& eng, const parser::Cmd& cmd) {
    return std::visit(MakeDispatcher<commands::FlattenLeaves<commands::RootGroup>>(eng), cmd);
}

parser::Token ClassifyRawArg(const std::string& text) {
    auto tokens = parser::Tokenize(text);
    if (tokens && tokens->size() == 2) {
        return tokens->front();
    }
    return parser::TkWord{text};
}

void SetupEngine(engine::Engine& eng) {
    eng.SetRawEvalFn([&eng](const std::vector<std::string>& args) -> io::Reply {
        std::vector<parser::Token> tokens;
        tokens.reserve(args.size() + 1);
        for (const auto& a : args)
            tokens.push_back(ClassifyRawArg(a));
        tokens.push_back(parser::TkEof{});
        auto cmd = parser::Parse(tokens);
        if (!cmd) {
            return io::MakeErr("ERR " + cmd.error().message);
        }
        return Eval(eng, *cmd);
    });
}

}  // namespace valkey::engine
