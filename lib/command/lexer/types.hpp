#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>

namespace valkey::parser {

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

using Token = std::variant<TkEof, TkWord, TkInt, TkFloat, TkStr>;

}  // namespace valkey::parser
