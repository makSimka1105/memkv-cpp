#pragma once

#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <system_error>

#include "cursor/char_cursor.hpp"
#include "lex/engine.hpp"
#include "script/error.hpp"
#include "script/token.hpp"

namespace valkey::script {

struct IntRule {
    static lex::Match<Token, ScriptError> Match(cursor::CharCursor& c) {
        if (!std::isdigit(static_cast<unsigned char>(c.Peek()))) {
            return std::nullopt;
        }

        size_t from = c.Pos();
        c.TakeWhile([](char ch) { return std::isdigit(static_cast<unsigned char>(ch)); });

        int64_t val{};
        auto sv = c.SliceFrom(from);
        auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);
        if (ec != std::errc{}) {
            return MakeError(ErrKind::kLex, "integer literal out of range");
        }
        return Token{TkInt{val}};
    }
};

}  // namespace valkey::script
