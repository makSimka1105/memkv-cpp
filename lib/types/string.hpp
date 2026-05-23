#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace valkey::types {

struct StringTrait {
    using Type = std::string;
    static constexpr std::string_view kName = "string";
    static size_t SizeOf(const Type& s) { return sizeof(Type) + s.size(); }
    static bool IsEmpty(const Type&) { return false; }
};

}  // namespace valkey::types
