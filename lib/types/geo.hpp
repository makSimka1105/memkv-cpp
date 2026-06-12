#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>

namespace valkey::types {

struct GeoPoint {
    double lon;
    double lat;
};

struct GeoTrait {
    using Type = std::unordered_map<std::string, GeoPoint>;
    static constexpr std::string_view kName = "geo";
    static size_t ElementBytes(std::string_view m) {
        return sizeof(std::string) + m.size() + sizeof(GeoPoint);
    }
    static size_t SizeOf(const Type& g) {
        size_t total = sizeof(Type);
        for (const auto& [m, _] : g) {
            total += ElementBytes(m);
        }
        return total;
    }
    static bool IsEmpty(const Type& g) { return g.empty(); }
};

}  // namespace valkey::types
