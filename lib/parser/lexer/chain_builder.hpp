#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include "IHandler.hpp"

namespace valkey::parser {

class ChainBuilder {
public:
    template <Handler H, typename... Args>
    ChainBuilder& Add(Args&&... args) {
        handlers_.push_back(std::make_unique<H>(std::forward<Args>(args)...));
        return *this;
    }

    std::unique_ptr<IHandler> Build() {
        if (handlers_.empty()) {
            return nullptr;
        }
        for (size_t i = handlers_.size() - 1; i > 0; --i) {
            handlers_[i - 1]->SetNext(std::move(handlers_[i]));
        }
        return std::move(handlers_[0]);
    }

private:
    std::vector<std::unique_ptr<IHandler>> handlers_;
};

}  // namespace valkey::parser
