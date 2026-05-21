#pragma once

#include <cstddef>
#include <string>

namespace valkey::parser {

struct ParseError {
    std::string message;
    size_t token_index = 0;
};

}  // namespace valkey::parser
