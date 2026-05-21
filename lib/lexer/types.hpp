#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>

namespace valkey::parser {

struct TkSkip {};
struct TkEof {};
struct TkWord {
    std::string text;
};
struct TkInt {
    int64_t value;
};
struct TkFloat {
    double value;
};
struct TkStr {
    std::string value;
};

using Token = std::variant<TkSkip, TkEof, TkWord, TkInt, TkFloat, TkStr>;

struct LexError {
    std::string message;
    size_t offset = 0;
};

struct LexMatch {
    Token token;
    size_t new_pos;
};

}  // namespace valkey::parser
