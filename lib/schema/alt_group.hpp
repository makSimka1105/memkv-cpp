#pragma once

#include <cstddef>
#include <expected>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>

#include "meta/overload.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "parser/token_convert.hpp"
#include "schema/apply.hpp"
#include "schema/command_trait.hpp"
#include "schema/contract.hpp"

namespace valkey {

template <FixedString Kw, class Node, auto EvalFn, schema::RuleFor<Node>... Rules>
struct AltCase {
    using NodeType = Node;
    static constexpr std::string_view kKeyword = Kw;
    static constexpr auto kEval = EvalFn;

    static bool Matches(std::string_view word) { return parser::CaseEqual(word, Kw); }

    static std::expected<Node, parser::ParseError> Parse(parser::Cursor& c) {
        if (!c.MatchKeyword(Kw)) {
            return Node{};
        }
        Node n{};
        auto r = schema::ApplyRules<Rules...>(c, n);
        if (!r) {
            return std::unexpected(r.error());
        }
        return n;
    }
};

template <class... Cases>
struct AltGroup {
    using Variant = std::variant<typename Cases::NodeType...>;

    static std::expected<Variant, parser::ParseError> Parse(parser::Cursor& c) {
        auto peeked = c.PeekString();
        if (!peeked) {
            return std::unexpected(parser::ParseError{"ERR syntax error", c.Pos()});
        }
        std::string_view word = *peeked;
        std::expected<Variant, parser::ParseError> out =
            std::unexpected(parser::ParseError{"ERR syntax error", c.Pos()});
        [&]<size_t... Is>(std::index_sequence<Is...>) {
            (void)([&]<size_t I>() -> bool {
                using C = std::tuple_element_t<I, std::tuple<Cases...>>;
                if (!C::Matches(word)) {
                    return false;
                }
                auto r = C::Parse(c);
                if (r) {
                    out = Variant{std::in_place_index<I>, std::move(*r)};
                } else {
                    out = std::unexpected(r.error());
                }
                return true;
            }.template operator()<Is>() || ...);
        }(std::make_index_sequence<sizeof...(Cases)>{});
        return out;
    }

    template <class... Args>
    static auto Eval(const Variant& v, Args&&... args) {
        return std::visit(
            Overload{[&](const typename Cases::NodeType& n) {
                return Cases::kEval(n, std::forward<Args>(args)...);
            }...},
            v);
    }
};

}  // namespace valkey
