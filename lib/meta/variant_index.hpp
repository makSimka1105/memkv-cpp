#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>
#include <variant>

namespace valkey::meta {

namespace detail {

template <typename T, typename V, size_t... Is>
constexpr size_t VariantIndexImpl(std::index_sequence<Is...>) {
    size_t found = sizeof...(Is);
    ((std::is_same_v<std::variant_alternative_t<Is, V>, T> ? (found = Is, 0) : 0), ...);
    return found;
}

}  // namespace detail

template <typename T, typename V>
constexpr size_t kVariantIndex =
    detail::VariantIndexImpl<T, V>(std::make_index_sequence<std::variant_size_v<V>>{});

}  // namespace valkey::meta
