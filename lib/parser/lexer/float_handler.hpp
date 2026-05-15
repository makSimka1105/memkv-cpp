#pragma once

#include <charconv>
#include <cstddef>
#include <optional>
#include <string_view>
#include <system_error>

#include "IHandler.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace valkey::parser {

class FloatHandler : public IHandler {
protected:
    HandleResult TryHandle(std::string_view input, size_t pos) override {
        size_t end = detail::WordEndPos(input, pos);
        std::string_view bare = input.substr(pos, end - pos);
        double value = 0.0;
        auto res = std::from_chars(bare.data(), bare.data() + bare.size(), value);
        if (res.ec != std::errc{} || res.ptr != bare.data() + bare.size()) {
            return std::nullopt;
        }
        return LexMatch{Token{TkFloat{value}}, end};
    }
};

}  // namespace valkey::parser
