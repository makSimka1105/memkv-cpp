#pragma once

#include <iostream>
#include <string>

#include "engine/engine.hpp"
#include "engine/session.hpp"
#include "engine/session_eval.hpp"
#include "io/human.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

namespace valkey::io {

void RunStdinRepl(engine::Engine& eng, std::istream& in, std::ostream& out) {
    engine::Session session;
    std::string line;
    while (std::getline(in, line)) {
        if (line == "EXIT" || line == "exit" || line == "QUIT" || line == "quit") {
            break;
        }

        auto tokens = parser::Tokenize(line);
        if (!tokens) {
            out << "(error) " << tokens.error().message << '\n';
            continue;
        }

        auto cmd = parser::Parse(*tokens);
        if (!cmd) {
            out << "(error) " << cmd.error().message << '\n';
            continue;
        }

        human::Write(out, engine::SessionEval(eng, session, *cmd));
    }
}

}  // namespace valkey::io
