#pragma once

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/registry.hpp"

namespace valkey::engine {

io::Reply Eval(Engine& eng, const parser::Cmd& cmd) {
    return parser::kCommands[cmd.index()].exec(eng, cmd);
}

}  // namespace valkey::engine
