#pragma once
#include <array>
#include <cstddef>
#include <expected>
#include <string_view>
#include "meta/fixed_string.hpp"
#include "command/parser/cursor.hpp"
#include "command/diagnostic.hpp"
namespace valkey::schema {
template <FixedString... Kws>
struct Enum {
    using Output = size_t;
    static constexpr size_t kNone = sizeof...(Kws);
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;
    static parser::CmdResult<size_t> Parse(parser::Cursor& cursor) {
        constexpr std::array<std::string_view, sizeof...(Kws)> kWs{std::string_view(Kws)...};
        for (size_t i = 0; i < kWs.size(); ++i) {
            if (cursor.MatchKeyword(kWs[i])) {
                return i;
            }
        }
        return kNone;
    }
    static constexpr bool MatchesKeyword(std::string_view word) {
        return (CaseEqual(word, Kws) || ...);
    }
    static size_t Default() { return kNone; }
};
}  // namespace valkey::schema
