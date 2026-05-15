#pragma once

#include <cctype>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

namespace valkey::engine {

struct Config {
    size_t maxmemory = 0;
};

std::optional<size_t> ParseMemorySize(std::string_view s) {
    if (s.empty()) {
        return std::nullopt;
    }

    size_t i = 0;
    size_t value = 0;
    while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) {
        value = value * 10 + static_cast<size_t>(s[i] - '0');
        ++i;
    }
    if (i == 0) {
        return std::nullopt;
    }

    std::string suffix;
    suffix.reserve(s.size() - i);
    for (size_t j = i; j < s.size(); ++j) {
        suffix.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(s[j]))));
    }

    constexpr size_t kKb = 1024;
    constexpr size_t kMb = kKb * 1024;
    constexpr size_t kGb = kMb * 1024;

    if (suffix.empty() || suffix == "b") {
        return value;
    }
    if (suffix == "kb") {
        return value * kKb;
    }
    if (suffix == "mb") {
        return value * kMb;
    }
    if (suffix == "gb") {
        return value * kGb;
    }
    return std::nullopt;
}

}  // namespace valkey::engine
