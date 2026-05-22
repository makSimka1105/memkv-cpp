#pragma once

#include <cstddef>
#include <expected>
#include <string_view>

#include "meta/command_trait.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "parser/token_convert.hpp"
#include "schema/apply.hpp"
#include "schema/contract.hpp"
#include "schema/scan.hpp"

namespace valkey::schema {

template <FixedString Kw, class T>
struct Keyword {
    using Output = T;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 2;

    static std::expected<T, parser::ParseError> Parse(parser::Cursor& c) {
        if (c.MatchKeyword(Kw)) {
            return ScanValue<T>(c);
        }
        return T{};
    }

    static constexpr bool MatchesKeyword(std::string_view word) {
        return parser::CaseEqual(word, Kw);
    }

    static T Default() { return T{}; }
};

template <FixedString Kw, class SubNode, RuleFor<SubNode>... Rules>
struct KeywordGroupStrategy {
    using Output = SubNode;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = SaturatingAdd(1, SaturatingSum(Rules::kMaxArgc...));

    static std::expected<SubNode, parser::ParseError> Parse(parser::Cursor& c) {
        if (!c.MatchKeyword(Kw)) {
            return SubNode{};
        }
        SubNode sub{};
        auto r = ApplyRules<Rules...>(c, sub);
        if (!r) {
            return std::unexpected(r.error());
        }
        return sub;
    }

    static constexpr bool MatchesKeyword(std::string_view word) {
        return parser::CaseEqual(word, Kw);
    }

    static SubNode Default() { return SubNode{}; }
};

template <FixedString Kw, class SubNode, class... Rules>
using KeywordGroup = KeywordGroupStrategy<Kw, SubNode, Rules...>;

}  // namespace valkey::schema
