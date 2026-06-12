#pragma once

#include <cstddef>
#include <expected>
#include <string_view>
#include <utility>

#include "command/parser/cursor.hpp"
#include "command/diagnostic.hpp"
#include "command/schema/core/concepts.hpp"

namespace valkey::schema {

template <Parser InnerParser>
struct Opt {
    using Output = typename InnerParser::Output;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = InnerParser::kMaxArgc;

    static parser::CmdResult<Output> Parse(parser::Cursor& cursor) {
        if (cursor.AtEnd()) {
            return Output{};
        }
        auto sp = cursor.Save();
        auto result = InnerParser::Parse(cursor);
        if (!result) {
            return Output{};
        }
        sp.Commit();
        return std::move(*result);
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
