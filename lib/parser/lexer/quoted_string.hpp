#pragma once

#include <cstddef>
#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "IHandler.hpp"
#include "utils.hpp"
#include "types.hpp"

namespace valkey::parser {

namespace detail {



}  // namespace detail

class QuotedStringHandler : public IHandler {
protected:
    HandleResult TryHandle(std::string_view input, size_t pos) override {
        if (pos >= input.size() || input[pos] != '"') {
            return std::nullopt;
        }
        std::string s;
        size_t j = pos + 1;
        while (j < input.size() && input[j] != '"') {
            if (input[j] == '\\' && j + 1 < input.size()) {
                auto ch = detail::ProcessEscape(input, j + 1);
                if (!ch.has_value()) {
                    return std::unexpected(ch.error());
                }
                s.push_back(*ch);
                j += 2;
            } else {
                s.push_back(input[j++]);
            }
        }
        if (j >= input.size()) {
            return std::unexpected(LexError{"unterminated string", pos});
        }
        return LexMatch{Token{TkStr{std::move(s)}}, j + 1};
    }
};

}  // namespace valkey::parser
