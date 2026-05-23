#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "meta/overload.hpp"
#include "structures/geo.hpp"
#include "structures/geohash.hpp"
#include "types/all.hpp"

namespace valkey::commands {

struct GeoFromMember {
    std::string member;
};

struct GeoFromLonLat {
    double lon{0.0};
    double lat{0.0};
};

struct GeoByRadius {
    double radius{0.0};
    std::string unit;
};

struct GeoByBox {
    double width{0.0};
    double height{0.0};
    std::string unit;
};

struct GeoHit {
    std::string member;
    double dist_m;
    double score;
    double lon;
    double lat;
};

using GeoFrom = std::variant<GeoFromMember, GeoFromLonLat>;
using GeoBy   = std::variant<GeoByRadius, GeoByBox>;

std::string_view UnitOf(const GeoBy& by) {
    return std::visit(Overload{
        [](const GeoByRadius& r) -> std::string_view { return r.unit; },
        [](const GeoByBox& b)-> std::string_view { return b.unit; }
    }, by);
}

std::optional<std::pair<double, double>> ResolveCenter(
    const GeoFrom& from, const types::ZSet& z) {
    return std::visit(Overload{
        [&](const GeoFromMember& m) -> std::optional<std::pair<double, double>> {
            auto it = z.scores.find(m.member);
            if (it == z.scores.end()) {
                return std::nullopt;
            }
            auto [lo, la] = structures::geohash::Decode(it->second);
            return std::pair{lo, la};
        },
        [](const GeoFromLonLat& p) -> std::optional<std::pair<double, double>> {
            return std::pair{p.lon, p.lat};
        }
    }, from);
}

bool IsInside(const GeoBy& by, double clon, double clat,
                     double mlon, double mlat, double dist_m) {
    return std::visit(Overload{
        [&](const GeoByRadius& r) -> bool {
            return dist_m <= structures::geo::ToMeters(r.radius, r.unit);
        },
        [&](const GeoByBox& b) -> bool {
            double hw = structures::geo::ToMeters(b.width, b.unit) / 2.0;
            double hh = structures::geo::ToMeters(b.height, b.unit) / 2.0;
            double dx = structures::geo::Haversine(clon, clat, mlon, clat);
            double dy = structures::geo::Haversine(clon, clat, clon, mlat);
            return dx <= hw && dy <= hh;
        }
    }, by);
}

std::vector<GeoHit> CollectHits(double clon, double clat, const GeoBy& by,
                                        const types::ZSet& z,
                                        std::optional<bool> sort_dir, int64_t count) {
    std::vector<GeoHit> hits;
    for (const auto& [member, score] : z.scores) {
        auto [mlon, mlat] = structures::geohash::Decode(score);
        double d = structures::geo::Haversine(clon, clat, mlon, mlat);
        if (IsInside(by, clon, clat, mlon, mlat, d)) {
            hits.push_back({member, d, score, mlon, mlat});
        }
    }
    if (sort_dir.has_value()) {
        if (*sort_dir) {
            std::ranges::sort(hits, {}, &GeoHit::dist_m);
        } else {
            std::ranges::sort(hits, std::ranges::greater{}, &GeoHit::dist_m);
        }
    }
    if (count > 0 && hits.size() > count) {
        hits.resize(count);
    }
    return hits;
}

}  // namespace valkey::commands
