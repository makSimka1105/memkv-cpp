#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <system_error>

#include "IHandler.hpp"
#include "utils.hpp"
#include "types.hpp"

namespace valkey::parser {

class IntHandler : public IHandler {
protected:
    HandleResult TryHandle(std::string_view input, size_t pos) override {
        size_t end = detail::WordEndPos(input, pos);
        std::string_view bare = input.substr(pos, end - pos);
        int64_t value = 0;
        auto res = std::from_chars(bare.data(), bare.data() + bare.size(), value);
        if (res.ec != std::errc{} || res.ptr != bare.data() + bare.size()) {
            return std::nullopt;
        }
        return LexMatch{Token{TkInt{value}}, end};
    }
};

}  // namespace valkey::parser
