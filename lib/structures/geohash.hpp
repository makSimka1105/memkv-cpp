#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

namespace valkey::structures::geohash {

static constexpr double kLonMin = -180.0;
static constexpr double kLonMax = 180.0;
static constexpr double kLatMin = -85.051129;
static constexpr double kLatMax = 85.051129;
static constexpr uint64_t kBits = 26;
static constexpr uint64_t kScale = 1ULL << kBits;

double Encode(double lon, double lat) {
    uint64_t lon_bits = static_cast<uint64_t>((lon - kLonMin) / (kLonMax - kLonMin) * kScale);
    uint64_t lat_bits = static_cast<uint64_t>((lat - kLatMin) / (kLatMax - kLatMin) * kScale);
    lon_bits = std::min(lon_bits, kScale - 1);
    lat_bits = std::min(lat_bits, kScale - 1);

    uint64_t hash = 0;
    for (uint64_t i = 0; i < kBits; ++i) {
        hash |= ((lat_bits >> i) & 1ULL) << (2 * i);
        hash |= ((lon_bits >> i) & 1ULL) << (2 * i + 1);
    }
    return static_cast<double>(hash);
}

std::pair<double, double> Decode(double score) {
    uint64_t hash = static_cast<uint64_t>(score);
    uint64_t lat_bits = 0;
    uint64_t lon_bits = 0;
    for (uint64_t i = 0; i < kBits; ++i) {
        lat_bits |= ((hash >> (2 * i)) & 1ULL) << i;
        lon_bits |= ((hash >> (2 * i + 1)) & 1ULL) << i;
    }
    double lat = static_cast<double>(lat_bits) / kScale * (kLatMax - kLatMin) + kLatMin;
    double lon = static_cast<double>(lon_bits) / kScale * (kLonMax - kLonMin) + kLonMin;
    return {lon, lat};
}

// Returns an 11-character standard base32 geohash string for the GEOHASH command.
std::string ToBase32(double score) {
    static constexpr std::string_view kAlphabet = "0123456789bcdefghjkmnpqrstuvwxyz";
    static constexpr int kChars = 11;

    auto [lon, lat] = Decode(score);

    double min_lon = kLonMin;
    double max_lon = kLonMax;
    double min_lat = -90.0;
    double max_lat = 90.0;

    std::array<int, kChars> bits{};
    bool is_lon = true;

    for (int i = 0; i < kChars * 5; ++i) {
        int char_idx = i / 5;
        int bit_pos = 4 - (i % 5);
        if (is_lon) {
            double mid = (min_lon + max_lon) / 2.0;
            if (lon >= mid) {
                bits[char_idx] |= (1 << bit_pos);
                min_lon = mid;
            } else {
                max_lon = mid;
            }
        } else {
            double mid = (min_lat + max_lat) / 2.0;
            if (lat >= mid) {
                bits[char_idx] |= (1 << bit_pos);
                min_lat = mid;
            } else {
                max_lat = mid;
            }
        }
        is_lon = !is_lon;
    }

    std::string result(kChars, '\0');
    for (int i = 0; i < kChars; ++i) {
        result[i] = kAlphabet[bits[i]];
    }
    return result;
}

}  // namespace valkey::structures::geohash
