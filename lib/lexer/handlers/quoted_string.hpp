#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

#include "lexer/handlers/IHandler.hpp"
#include "lexer/handlers/utils.hpp"
#include "meta/try.hpp"

namespace valkey::parser {

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
                VK_ASSIGN_OR_RETURN(char ch, detail::ProcessEscape(input, j + 1));
                s.push_back(ch);
                j += 2;
            } else {
                s.push_back(input[j++]);
            }
        }
        if (j >= input.size()) {
            return std::unexpected(DiagnosticMessage{"unterminated string"});
        }
        return LexMatch{Token{TkStr{std::move(s)}}, j + 1};
    }
};

}  // namespace valkey::parser
