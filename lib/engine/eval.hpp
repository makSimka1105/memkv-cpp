#pragma once


#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/overload.hpp"
#include "meta/type_list.hpp"

namespace valkey::engine {

namespace detail {

template <class List>
struct DispatcherFactory;

template <class... Traits>
struct DispatcherFactory<TraitsList<Traits...>> {
    static auto Make(engine::Engine& eng) {
        return Overload{
            [&eng](const typename Traits::Type& c) -> io::Reply { return Traits::kExecFn(eng, c); }...};
    }
};

}  // namespace detail

template <class List>
auto MakeDispatcher(engine::Engine& eng) {
    return detail::DispatcherFactory<List>::Make(eng);
}



}  // namespace valkey::engine
