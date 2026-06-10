#pragma once

#include <string_view>

namespace valkey::script {

struct Cursor {
    std::string_view src;
    size_t pos = 0;

    char Peek(size_t n = 0) const {
        return pos + n < src.size() ? src[pos + n] : '\0';
    }

    void Advance(size_t n = 1) { pos += n; }

    bool AtEnd() const { return pos >= src.size(); }

    std::string_view SliceFrom(size_t from) const {
        return src.substr(from, pos - from);
    }

    std::string_view TakeWhile(auto pred) {
        size_t from = pos;
        while (!AtEnd() && pred(Peek())) Advance();
        return SliceFrom(from);
    }

    bool MatchPrefix(std::string_view sv) {
        if (src.substr(pos).starts_with(sv)) {
            pos += sv.size();
            return true;
        }
        return false;
    }
};

}  // namespace valkey::script
