#pragma once

namespace valkey::meta {



template <class... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};

template <typename... Ts>
Overload(Ts...) -> Overload<Ts...>;






}  // namespace valkey::meta
