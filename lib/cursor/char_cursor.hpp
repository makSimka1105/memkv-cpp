#pragma once

#include <cstddef>
#include <string_view>

#include "cursor/base_cursor.hpp"

namespace valkey::cursor {

class CharCursor : public CursorBase<char> {
public:
    explicit CharCursor(std::string_view src) : CursorBase<char>({src.data(), src.size()}) {}

    char Peek(size_t n = 0) const {
        const char* p = At(n);
        return p ? *p : '\0';
    }

    std::string_view SliceFrom(size_t from) const { return {items_.data() + from, pos_ - from}; }

    std::string_view PeekWhile(auto pred) const {
        size_t end = pos_;
        while (end < items_.size() && pred(items_[end])) {
            ++end;
        }
        return {items_.data() + pos_, end - pos_};
    }

    std::string_view TakeWhile(auto pred) {
        std::string_view run = PeekWhile(pred);
        Advance(run.size());
        return run;
    }

    bool StartsWith(std::string_view sv) const {
        if (pos_ + sv.size() > items_.size()) {
            return false;
        }
        return std::string_view(items_.data() + pos_, sv.size()) == sv;
    }

    bool MatchPrefix(std::string_view sv) {
        if (StartsWith(sv)) {
            Advance(sv.size());
            return true;
        }
        return false;
    }
};

}  // namespace valkey::cursor
