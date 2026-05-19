#pragma once

#include "io/reply.hpp"
#include "meta/overload.hpp"
#include "meta/type_list.hpp"

namespace valkey::engine {
class Engine;
} // namespace valkey::engine

namespace valkey::meta {

template <class... Traits>
auto MakeDispatcher(engine::Engine& eng, TraitsList<Traits...>) {
    return Overload{
        [&eng](const typename Traits::Type& c) -> io::Reply { return Traits::kExecFn(eng, c); }...};
}

}  // namespace valkey::meta
