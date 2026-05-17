#pragma once

#include <type_traits>
#include <variant>

namespace valkey::meta {

template <class... Traits>
struct TraitsList {};

namespace detail {

template <class L>
struct VariantFromListImpl;

template <class... Traits>
struct VariantFromListImpl<TraitsList<Traits...>> {
    using type = std::variant<typename Traits::Type...>;
};

template <class T, class L>
struct TraitForImpl;

template <class T>
struct TraitForImpl<T, TraitsList<>> {};

template <class T, class Head, class... Tail>
struct TraitForImpl<T, TraitsList<Head, Tail...>> {
    using type = typename std::conditional_t<std::is_same_v<T, typename Head::Type>,
                                             std::type_identity<Head>,
                                             TraitForImpl<T, TraitsList<Tail...>>>::type;
};

}  // namespace detail

template <class L>
using VariantFromList = typename detail::VariantFromListImpl<L>::type;

template <class T, class L>
using TraitFor = typename detail::TraitForImpl<T, L>::type;

}  // namespace valkey::meta
