#pragma once

#include <iostream>
#include <string>

#include "engine/engine.hpp"
#include "engine/eval.hpp"
#include "engine/session.hpp"
#include "engine/session_eval.hpp"
#include "io/human.hpp"
#include "io/reply.hpp"
#include "command/lexer/lexer.hpp"
#include "command/parser/parser.hpp"

namespace valkey::io {

void RunStdinRepl(engine::Engine& eng, std::istream& in, std::ostream& out) {
    engine::SetupEngine(eng);
    engine::Session session;
    std::string line;
    while (std::getline(in, line)) {
        if (line == "EXIT" || line == "exit" || line == "QUIT" || line == "quit") {
            break;
        }

        auto tokens = parser::Tokenize(line);
        if (!tokens) {
            human::Write(out, io::MakeErr(tokens.error().message));
            continue;
        }

        auto cmd = parser::Parse(*tokens);
        if (!cmd) {
            human::Write(out, io::MakeErr(cmd.error().message));
            continue;
        }

        human::Write(out, engine::SessionEval(eng, session, *cmd));
    }
}

}  // namespace valkey::io
