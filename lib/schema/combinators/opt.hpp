#pragma once

#include <cstddef>
#include <expected>
#include <string_view>

#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "schema/core/concepts.hpp"

namespace valkey::schema {

template <Parser InnerParser>
struct Opt {
    using Output = typename InnerParser::Output;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = InnerParser::kMaxArgc;

    static std::expected<Output, parser::ParseError> Parse(parser::Cursor& cursor) {
        if (cursor.Done()) {
            return Output{};
        }
        size_t saved_pos = cursor.pos;
        auto result = InnerParser::Parse(cursor);
        if (!result) {
            cursor.pos = saved_pos;
            return Output{};
        }
        return *result;
    }

    static constexpr bool MatchesKeyword(std::string_view word)
        requires LabeledParser<InnerParser>
    {
        return InnerParser::MatchesKeyword(word);
    }

    static Output Default() {
        if constexpr (LabeledParser<InnerParser>) {
            return InnerParser::Default();
        } else {
            return Output{};
        }
    }
};

}  // namespace valkey::schema
