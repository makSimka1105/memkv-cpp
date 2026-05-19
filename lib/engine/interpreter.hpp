#pragma once

#include <variant>

#include "commands/all.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/dispatcher.hpp"
#include "parser/cmd.hpp"

namespace valkey::engine {

io::Reply Eval(Engine& eng, const parser::Cmd& cmd) {
    return std::visit(meta::MakeDispatcher(eng, commands::AllCommandsTraits{}), cmd);
}

}  // namespace valkey::engine
