#pragma once

#include <concepts>
#include <cstddef>
#include <expected>
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "cursor/char_cursor.hpp"
#include "meta/match_first.hpp"

namespace valkey::lex {

template <class C>
concept LexCursor = requires(C c, size_t n) {
    { c.Peek(n) } -> std::same_as<char>;
    { c.AtEnd() } -> std::same_as<bool>;
    { c.Pos() } -> std::convertible_to<size_t>;
    c.Advance(n);
};

template <class Tok, class Err>
using Match = std::optional<std::expected<Tok, Err>>;

template <class... Rules>
struct RuleSet {
    template <LexCursor C>
    static auto Try(C& c) {
        using First = std::tuple_element_t<0, std::tuple<Rules...>>;
        std::remove_cvref_t<decltype(First::Match(c))> hit;
        MatchFirst<Rules...>([&]<std::size_t, class R>() {
            if (auto r = R::Match(c)) {
                hit = std::move(r);
                return true;
            }
            return false;
        });
        return hit;
    }
};

template <class Spec>
class Lexer {
    using Tok = typename Spec::Token;
    using Err = typename Spec::Error;

public:
    static std::expected<std::vector<Tok>, Err> Run(std::string_view src) {
        typename Spec::Cursor c{src};
        std::vector<Tok> out;
        while (true) {
            c.TakeWhile([](char ch) { return Spec::Skip(ch); });
            if (c.AtEnd()) {
                break;
            }
            auto hit = Spec::Rules::Try(c);
            if (!hit) {
                return std::unexpected(Spec::OnUnexpected(c.Peek()));
            }
            if (!hit->has_value()) {
                return std::unexpected(std::move(hit->error()));
            }
            out.push_back(std::move(hit->value()));
        }
        out.push_back(Spec::Eof());
        return out;
    }
};

}  // namespace valkey::lex
