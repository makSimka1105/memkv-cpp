#pragma once

#include <variant>

#include "commands/all.hpp"
#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "meta/overload.hpp"
#include "meta/type_list.hpp"
#include "parser/cmd.hpp"

namespace valkey::engine {

template <class... Traits>
auto MakeDispatcher(Engine& eng, TraitsList<Traits...>) {
    return Overload{
        [&eng](const typename Traits::Type& c) -> io::Reply { return Traits::kExecFn(eng, c); }...};
}

io::Reply Eval(Engine& eng, const parser::Cmd& cmd) {
    return std::visit(MakeDispatcher(eng, FlattenLeaves<commands::RootGroup>{}), cmd);
}

}  // namespace valkey::engine
