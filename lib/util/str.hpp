#pragma once
#include <cstddef>
#include <string>
#include <string_view>

namespace valkey {

constexpr char ToUpper(char c) {
    return (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c;
}

constexpr bool CaseEqual(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (ToUpper(a[i]) != ToUpper(b[i])) {
            return false;
        }
    }
    return true;
}

std::string LowerName(std::string_view name) {
    std::string out;
    out.reserve(name.size());
    for (char c : name) {
        out.push_back((c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c);
    }
    return out;
}

}  // namespace valkey
