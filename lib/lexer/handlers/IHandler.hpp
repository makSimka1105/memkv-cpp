#pragma once

#include <concepts>
#include <cstddef>
#include <expected>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>

#include "lexer/types.hpp"

namespace valkey::parser {

using HandleResult = std::expected<std::optional<LexMatch>, LexError>;

class IHandler {
public:
    virtual ~IHandler() = default;

    void SetNext(std::unique_ptr<IHandler> next) { next_ = std::move(next); }

    HandleResult Handle(std::string_view input, size_t pos) {
        auto result = TryHandle(input, pos);
        if (!result.has_value() || result->has_value()) {
            return result;
        }
        if (next_) {
            return next_->Handle(input, pos);
        }
        return std::nullopt;
    }

protected:
    virtual HandleResult TryHandle(std::string_view input, size_t pos) = 0;

private:
    std::unique_ptr<IHandler> next_;
};

template <typename T>
concept Handler = std::derived_from<T, IHandler>;

}  // namespace valkey::parser
