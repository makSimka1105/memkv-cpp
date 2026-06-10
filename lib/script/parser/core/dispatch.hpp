#pragma once

#include <cstddef>
#include <optional>
#include <utility>

#include "meta/match_first.hpp"
#include "script/parser/core/result.hpp"
#include "script/parser/cursor.hpp"

namespace valkey::script {

template <Dispatchable... Ts>
struct ParseletSet {
    template <typename Out, typename Driver>
    static std::optional<ParseResult<Out>> TryParse(TokenCursor& cursor, Driver& driver) {
        std::optional<ParseResult<Out>> hit;
        MatchFirst<Ts...>([&]<size_t, typename T>() {
            if (!T::Matches(cursor.Peek())) {
                return false;
            }
            hit = T::Parse(cursor, driver);
            return true;
        });
        return hit;
    }
};

}  // namespace valkey::script
