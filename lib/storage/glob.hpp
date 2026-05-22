#pragma once

#include <cstddef>
#include <string_view>

namespace valkey::storage {

bool GlobMatch(std::string_view pattern, std::string_view text) {
    size_t pi = 0, ti = 0;
    size_t star_pi = std::string_view::npos;
    size_t star_ti = 0;
    while (ti < text.size()) {
        if (pi < pattern.size() && (pattern[pi] == '?' || pattern[pi] == text[ti])) {
            ++pi;
            ++ti;
        } else if (pi < pattern.size() && pattern[pi] == '*') {
            star_pi = pi++;
            star_ti = ti;
        } else if (star_pi != std::string_view::npos) {
            pi = star_pi + 1;
            ti = ++star_ti;
        } else {
            return false;
        }
    }
    while (pi < pattern.size() && pattern[pi] == '*') {
        ++pi;
    }
    return pi == pattern.size();
}

}  // namespace valkey::storage
