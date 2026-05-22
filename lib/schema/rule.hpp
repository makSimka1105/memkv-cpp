#pragma once

#include <concepts>
#include <cstddef>
#include <expected>
#include <string_view>
#include <tuple>
#include <utility>

#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "schema/contract.hpp"

namespace valkey::schema {

template <class S>
concept KeywordStrategy = Strategy<S> && requires(std::string_view kw) {
    { S::MatchesKeyword(kw) } -> std::convertible_to<bool>;
    { S::Default() } -> std::same_as<typename S::Output>;
};

template <class S, auto MemPtr>
struct BoundRule {
    static constexpr size_t kMinArgc = S::kMinArgc;
    static constexpr size_t kMaxArgc = S::kMaxArgc;

    template <class Node>
    static std::expected<void, parser::ParseError> Apply(parser::Cursor& c, Node& node) {
        auto r = S::Parse(c);
        if (!r) {
            return std::unexpected(r.error());
        }
        node.*MemPtr = std::move(*r);
        return {};
    }

    static constexpr bool MatchesKeyword(std::string_view word)
        requires KeywordStrategy<S>
    {
        return S::MatchesKeyword(word);
    }

    template <class Node>
    static void ApplyDefault(Node& node)
        requires KeywordStrategy<S>
    {
        node.*MemPtr = S::Default();
    }
};

template <auto MemPtr, class S>
using Bind = BoundRule<S, MemPtr>;

template <class... Rules>
struct AnyOrderRule {
    static constexpr size_t kMinArgc = SaturatingSum(Rules::kMinArgc...);
    static constexpr size_t kMaxArgc = SaturatingSum(Rules::kMaxArgc...);

    template <class Node>
    static std::expected<void, parser::ParseError> Apply(parser::Cursor& c, Node& node) {
        static_assert((OptionalRule<Rules, Node> && ...),
                      "AnyOrder requires every rule to be an OptionalRule for Node");
        bool used[sizeof...(Rules)] = {};
        std::expected<void, parser::ParseError> applied{};
        while (!c.Done()) {
            auto peek = c.PeekString();
            if (!peek) {
                break;
            }
            std::string_view word = *peek;
            bool matched = false;
            [&]<size_t... Is>(std::index_sequence<Is...>) {
                (void)([&]<size_t I>() -> bool {
                    if (used[I]) {
                        return false;
                    }
                    using Rule = std::tuple_element_t<I, std::tuple<Rules...>>;
                    if (!Rule::MatchesKeyword(word)) {
                        return false;
                    }
                    applied = Rule::Apply(c, node);
                    used[I] = true;
                    matched = true;
                    return true;
                }.template operator()<Is>() || ...);
            }(std::make_index_sequence<sizeof...(Rules)>{});
            if (!matched) {
                break;
            }
            if (!applied) {
                return std::unexpected(applied.error());
            }
        }
        [&]<size_t... Is>(std::index_sequence<Is...>) {
            ((used[Is]
                  ? void()
                  : (std::tuple_element_t<Is, std::tuple<Rules...>>::ApplyDefault(node), void())),
             ...);
        }(std::make_index_sequence<sizeof...(Rules)>{});
        return {};
    }
};

template <class... Rules>
using AnyOrder = AnyOrderRule<Rules...>;

}  // namespace valkey::schema
