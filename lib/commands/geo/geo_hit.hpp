#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "commands/geo/geo_by.hpp"
#include "meta/overload.hpp"
#include "structures/geo.hpp"
#include "structures/geohash.hpp"
#include "types/all.hpp"

namespace valkey::commands {

struct GeoHit {
    std::string member;
    double dist_m;
    double score;
    double lon;
    double lat;
};

std::vector<GeoHit> CollectHits(double clon, double clat, const GeoBy::Type& by,
                                       const types::ZSet& z,
                                       std::optional<bool> sort_dir, int64_t count) {
    std::vector<GeoHit> hits;
    for (const auto& [member, score] : z.scores) {
        auto [mlon, mlat] = structures::geohash::Decode(score);
        double d = structures::geo::Haversine(clon, clat, mlon, mlat);
        bool inside = std::visit(Overload{
            [&](const GeoRadiusFilter& r) { return RadiusContains(r, clon, clat, mlon, mlat, d); },
            [&](const GeoBoxFilter& b) { return BoxContains(b, clon, clat, mlon, mlat, d); },
        }, by);
        if (inside) {
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
    if (count > 0 && hits.size() > static_cast<size_t>(count)) {
        hits.resize(static_cast<size_t>(count));
    }
    return hits;
}

}  // namespace valkey::commands
