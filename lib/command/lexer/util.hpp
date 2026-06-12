#pragma once

#include "lex/char_class.hpp"

namespace valkey::parser::detail {

bool IsWordChar(char c) {
    return !lex::IsSpace(c) && c != '"';
}

}  // namespace valkey::parser::detail
