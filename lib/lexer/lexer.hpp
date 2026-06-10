#pragma once

#include <cstddef>
#include <expected>
#include <memory>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "diagnostic.hpp"
#include "lexer/handlers/IHandler.hpp"
#include "lexer/handlers/chain_builder.hpp"
#include "lexer/handlers/float_handler.hpp"
#include "lexer/handlers/int_handler.hpp"
#include "lexer/handlers/quoted_string.hpp"
#include "lexer/handlers/whitespace.hpp"
#include "lexer/handlers/word_handler.hpp"
#include "lexer/types.hpp"
#include "meta/try.hpp"

namespace valkey::parser {

class Lexer {
public:
    explicit Lexer(std::unique_ptr<IHandler> chain) : chain_(std::move(chain)) {}

    std::expected<std::vector<Token>, DiagnosticMessage> operator()(std::string_view input) const {
        std::vector<Token> tokens;
        size_t pos = 0;
        while (pos < input.size()) {
            VK_ASSIGN_OR_RETURN(auto match_opt, chain_->Handle(input, pos));
            if (!match_opt.has_value()) {
                return std::unexpected(DiagnosticMessage{"no handler matched"});
            }
            auto& [token, new_pos] = *match_opt;
            if (!std::holds_alternative<TkSkip>(token)) {
                tokens.push_back(std::move(token));
            }
            pos = new_pos;
        }
        tokens.emplace_back(TkEof{});
        return tokens;
    }

private:
    std::unique_ptr<IHandler> chain_;
};

const Lexer& DefaultLexer() {
    static const Lexer kLexer{ChainBuilder{}
                                  .Add<WhitespaceHandler>()
                                  .Add<QuotedStringHandler>()
                                  .Add<IntHandler>()
                                  .Add<FloatHandler>()
                                  .Add<WordHandler>()
                                  .Build()};
    return kLexer;
}

std::expected<std::vector<Token>, DiagnosticMessage> Tokenize(std::string_view input,
                                                              const Lexer& lexer = DefaultLexer()) {
    return lexer(input);
}

}  // namespace valkey::parser
