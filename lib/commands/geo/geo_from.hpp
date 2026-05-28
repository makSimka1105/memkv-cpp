#pragma once

#include <optional>
#include <string>
#include <utility>

#include "schema/alt_group.hpp"
#include "schema/rule.hpp"
#include "schema/strategies/arg.hpp"
#include "structures/geohash.hpp"
#include "types/all.hpp"

namespace valkey::commands {

struct GeoMemberRef {
    std::string member;
};

struct GeoCoordinate {
    double lon{0.0};
    double lat{0.0};
};

using GeoCenter = std::optional<std::pair<double, double>>;

inline GeoCenter CenterFromMember(const GeoMemberRef& m, const types::ZSet& z) {
    auto it = z.scores.find(m.member);
    if (it == z.scores.end()) {
        return std::nullopt;
    }
    auto [lo, la] = structures::geohash::Decode(it->second);
    return std::pair{lo, la};
}

inline GeoCenter CenterFromCoord(const GeoCoordinate& p, const types::ZSet&) {
    return std::pair{p.lon, p.lat};
}

using GeoFromGroup = AltGroup<
    AltCase<"FROMMEMBER", GeoMemberRef, &CenterFromMember,
        schema::Bind<&GeoMemberRef::member, schema::Arg<std::string>>>,
    AltCase<"FROMLONLAT", GeoCoordinate, &CenterFromCoord,
        schema::Bind<&GeoCoordinate::lon, schema::Arg<double>>,
        schema::Bind<&GeoCoordinate::lat, schema::Arg<double>>>>;

using GeoFrom = GeoFromGroup::Variant;

}  // namespace valkey::commands
