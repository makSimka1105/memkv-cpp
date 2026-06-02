#pragma once

#include <cstddef>
#include <expected>
#include <utility>
#include <vector>

#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "schema/combinators/apply_fields.hpp"
#include "schema/core/argc.hpp"
#include "schema/core/concepts.hpp"

namespace valkey::schema {

template <class Elem, FieldFor<Elem>... Fields>
struct Repeat {
    using Output = std::vector<Elem>;
    static constexpr size_t kMinArgc = SaturatingSum(Fields::kMinArgc...);
    static constexpr size_t kMaxArgc = kUnbounded;

    static std::expected<std::vector<Elem>, parser::ParseError> Parse(parser::Cursor& cursor) {
        std::vector<Elem> out_elements;
        while (!cursor.Done()) {
            Elem element{};
            size_t start_pos = cursor.pos;
            auto result = ApplyFields<Fields...>(cursor, element);
            if (!result) {
                return std::unexpected(result.error());
            }
            if (cursor.pos == start_pos) {
                break;
            }
            out_elements.push_back(std::move(element));
        }
        return out_elements;
    }
};

}  // namespace valkey::schema
