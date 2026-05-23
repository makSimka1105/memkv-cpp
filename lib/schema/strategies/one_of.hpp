#pragma once

#include <algorithm>
#include <cstddef>
#include <expected>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>

#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "schema/contract.hpp"

namespace valkey::schema {

template <class... Groups>
struct OneOfStrategy {
    using Output = std::variant<typename Groups::Output...>;
    static constexpr size_t kMinArgc = std::min({Groups::kGroupMinArgc...});
    static constexpr size_t kMaxArgc = std::max({Groups::kGroupMaxArgc...});

    static std::expected<Output, parser::ParseError> Parse(parser::Cursor& c) {
        auto peeked = c.PeekString();
        if (!peeked) {
            return std::unexpected(parser::ParseError{"ERR syntax error", c.Pos()});
        }
        std::string_view word = *peeked;
        std::expected<Output, parser::ParseError> result =
            std::unexpected(parser::ParseError{"ERR syntax error", c.Pos()});
        [&]<size_t... Is>(std::index_sequence<Is...>) {
            (void)([&]<size_t I>() -> bool {
                using Group = std::tuple_element_t<I, std::tuple<Groups...>>;
                if (!Group::MatchesKeyword(word)) {
                    return false;
                }
                auto r = Group::Parse(c);
                if (!r) {
                    result = std::unexpected(r.error());
                } else {
                    result = Output{std::in_place_index<I>, std::move(*r)};
                }
                return true;
            }.template operator()<Is>() || ...);
        }(std::make_index_sequence<sizeof...(Groups)>{});
        return result;
    }
};

template <class... Groups>
using OneOf = OneOfStrategy<Groups...>;

}  // namespace valkey::schema
