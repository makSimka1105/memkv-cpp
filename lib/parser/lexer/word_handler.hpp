#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "IHandler.hpp"
#include "utils.hpp"
#include "types.hpp"

namespace valkey::parser {

class WordHandler : public IHandler {
protected:
    HandleResult TryHandle(std::string_view input, size_t pos) override {
        size_t end = detail::WordEndPos(input, pos);
        return LexMatch{Token{TkWord{std::string(input.substr(pos, end - pos))}}, end};
    }
};

}  // namespace valkey::parser
