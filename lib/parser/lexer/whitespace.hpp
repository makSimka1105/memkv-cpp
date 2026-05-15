#pragma once

#include <cstddef>
#include <optional>
#include <string_view>

#include "IHandler.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace valkey::parser {

class WhitespaceHandler : public IHandler {
protected:
    HandleResult TryHandle(std::string_view input, size_t pos) override {
        if (pos >= input.size() || !detail::IsWhitespace(input[pos])) {
            return std::nullopt;
        }
        size_t end = pos;
        while (end < input.size() && detail::IsWhitespace(input[end])) {
            ++end;
        }
        return LexMatch{TkSkip{}, end};
    }
};

}  // namespace valkey::parser
