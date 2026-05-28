#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "commands/geo/geo_by.hpp"
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

inline std::vector<GeoHit> CollectHits(double clon, double clat, const GeoBy& by,
                                       const types::ZSet& z,
                                       std::optional<bool> sort_dir, int64_t count) {
    std::vector<GeoHit> hits;
    for (const auto& [member, score] : z.scores) {
        auto [mlon, mlat] = structures::geohash::Decode(score);
        double d = structures::geo::Haversine(clon, clat, mlon, mlat);
        if (GeoByGroup::Eval(by, clon, clat, mlon, mlat, d)) {
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
