#pragma once
#include <cstddef>
#include <functional>
#include <string_view>

namespace valkey {

struct StringHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }
};

}  // namespace valkey
