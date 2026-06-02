#pragma once

#include <variant>

#include "all.hpp"
#include "engine/eval.hpp"
#include "commands/tree.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "parser/cmd.hpp"

namespace valkey::engine {

io::Reply Eval(engine::Engine& eng, const parser::Cmd& cmd) {
    return std::visit(MakeDispatcher<commands::FlattenLeaves<commands::RootGroup>>(eng),
                      cmd);
}

}  // namespace valkey::engine
