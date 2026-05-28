#pragma once

#include <string_view>

#include "meta/type_list.hpp"
#include "schema/command_trait.hpp"

namespace valkey {

template <FixedString Name, class Schema, auto ExecFn>
struct CommandTrait {
    static constexpr std::string_view kName = Name;
    using Type = typename Schema::NodeType;
    using SchemaType = Schema;
    static constexpr auto kExecFn = ExecFn;
};

template <class Trait>
struct LeafCommand {
    static constexpr bool kIsGroup = false;
    static constexpr std::string_view kName = Trait::kName;
    using Type = typename Trait::Type;
    using SchemaType = typename Trait::SchemaType;
    static constexpr auto kExecFn = Trait::kExecFn;
};

template <FixedString Name, class... Children>
struct CommandGroup {
    static constexpr bool kIsGroup = true;
    static constexpr std::string_view kName = Name;
    using ChildrenList = TraitsList<Children...>;
};

namespace detail {

template <class A, class B>
struct ListCat;

template <class... A, class... B>
struct ListCat<TraitsList<A...>, TraitsList<B...>> {
    using type = TraitsList<A..., B...>;
};

template <class... Lists>
struct ConcatAll;

template <>
struct ConcatAll<> {
    using type = TraitsList<>;
};

template <class Head, class... Tail>
struct ConcatAll<Head, Tail...> {
    using type = typename ListCat<Head, typename ConcatAll<Tail...>::type>::type;
};

template <class T>
struct FlattenImpl;

template <class Trait>
struct FlattenImpl<LeafCommand<Trait>> {
    using type = TraitsList<LeafCommand<Trait>>;
};

template <FixedString Name, class... Children>
struct FlattenImpl<CommandGroup<Name, Children...>> {
    using type = typename ConcatAll<typename FlattenImpl<Children>::type...>::type;
};

}  // namespace detail

template <class T>
using FlattenLeaves = typename detail::FlattenImpl<T>::type;

}  // namespace valkey
