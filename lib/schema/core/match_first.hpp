#pragma once
#include <cstddef>
#include <utility>
namespace valkey::schema {
template <class... Types, class Visitor>
constexpr bool MatchFirst(Visitor&& visitor) {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return (visitor.template operator()<Is, Types>() || ...);
    }(std::index_sequence_for<Types...>{});
}
}  // namespace valkey::schema
