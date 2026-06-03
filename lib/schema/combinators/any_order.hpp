#pragma once

#include <cstddef>
#include <expected>
#include <string_view>
#include <utility>

#include "parser/cursor.hpp"
#include "parser/errors.hpp"
#include "meta/try.hpp"
#include "schema/combinators/required.hpp"
#include "schema/core/argc.hpp"
#include "schema/core/concepts.hpp"
#include "schema/core/match_first.hpp"

namespace valkey::schema {

template <class... Fields>
struct AnyOrder {
    static constexpr size_t kMinArgc =
        SaturatingSum((FieldRequired<Fields>() ? Fields::kMinArgc : size_t{0})...);
    static constexpr size_t kMaxArgc = SaturatingSum(Fields::kMaxArgc...);

    template <class Node>
        requires(LabeledField<Fields, Node> && ...)
    static std::expected<void, parser::ParseError> Apply(parser::Cursor& cursor, Node& node) {
        bool used[sizeof...(Fields)] = {};
        std::expected<void, parser::ParseError> applied{};
        while (!cursor.Done()) {
            auto peeked_word = cursor.PeekStringView();
            if (!peeked_word) {
                break;
            }
            std::string_view word = *peeked_word;
            bool matched = MatchFirst<Fields...>([&]<size_t I, class Field>() -> bool {
                if (used[I] || !Field::MatchesKeyword(word)) {
                    return false;
                }
                applied = Field::Apply(cursor, node);
                used[I] = true;
                return true;
            });
            if (!matched) {
                break;
            }
            VK_RETURN_IF_ERROR(applied);
        }

        std::expected<void, parser::ParseError> result{};
        [&]<size_t... Is>(std::index_sequence<Is...>) {
            auto finalize = [&]<size_t I, class Field>() {
                if (used[I]) {
                    return;
                }
                if constexpr (FieldRequired<Field>()) {
                    if (result) {
                        result =
                            parser::err::SyntaxError(cursor.Pos());
                    }
                } else {
                    Field::ApplyDefault(node);
                }
            };
            (finalize.template operator()<Is, Fields>(), ...);
        }(std::index_sequence_for<Fields...>{});
        return result;
    }

    static constexpr bool MatchesKeyword(std::string_view word) {
        return (Fields::MatchesKeyword(word) || ...);
    }

    template <class Node>
    static void ApplyDefault(Node& node) {
        (Fields::ApplyDefault(node), ...);
    }
};

}  // namespace valkey::schema
