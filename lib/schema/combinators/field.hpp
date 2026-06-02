#pragma once

#include <cstddef>
#include <expected>
#include <string_view>
#include <utility>

#include "parser/cursor.hpp"
#include "parser/errors.hpp"
#include "schema/core/concepts.hpp"

namespace valkey::schema {

template <auto MemPtr, Parser InnerParser>
struct Field {
    static constexpr size_t kMinArgc = InnerParser::kMinArgc;
    static constexpr size_t kMaxArgc = InnerParser::kMaxArgc;

    template <class Node>
    static std::expected<void, parser::ParseError> Apply(parser::Cursor& c, Node& node) {
        return InnerParser::Parse(c).transform([&](auto&& v) { node.*MemPtr = std::move(v); });
    }

    static constexpr bool MatchesKeyword(std::string_view word)
        requires LabeledParser<InnerParser>
    {
        return InnerParser::MatchesKeyword(word);
    }

    template <class Node>
    static void ApplyDefault(Node& node)
        requires LabeledParser<InnerParser>
    {
        node.*MemPtr = InnerParser::Default();
    }
};

}  // namespace valkey::schema
