#pragma once

#include <cstddef>
#include <variant>

#include "cursor/base_cursor.hpp"

namespace valkey::cursor {

template <class Tok>
class TokenCursor : public CursorBase<Tok> {
public:
    using CursorBase<Tok>::CursorBase;

    const Tok& Peek(size_t offset = 0) const {
        const Tok* p = this->At(offset);
        return p ? *p : this->items_.back();
    }

    template <class U>
    bool Is(size_t offset = 0) const {
        return std::holds_alternative<U>(Peek(offset));
    }

    template <class U>
    const U* As(size_t offset = 0) const {
        return std::get_if<U>(&Peek(offset));
    }

    Tok Advance() {
        const Tok* p = this->At();
        if (p) {
            CursorBase<Tok>::Advance(1);
            return *p;
        }
        return this->items_.back();
    }
};

}  // namespace valkey::cursor
