#pragma once

#include <optional>
#include <string>
#include <utility>

#include "schema/combinators/field.hpp"
#include "schema/combinators/group.hpp"
#include "schema/combinators/one_of.hpp"
#include "schema/parsers/arg.hpp"
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

GeoCenter CenterFromMember(const GeoMemberRef& m, const types::ZSet& z) {
    auto it = z.scores.find(m.member);
    if (it == z.scores.end()) {
        return std::nullopt;
    }
    auto [lo, la] = structures::geohash::Decode(it->second);
    return std::pair{lo, la};
}

GeoCenter CenterFromCoord(const GeoCoordinate& p, const types::ZSet&) {
    return std::pair{p.lon, p.lat};
}

using GeoFrom = schema::OneOf<
    schema::Case<"FROMMEMBER", GeoMemberRef,
        schema::Field<&GeoMemberRef::member, schema::Arg<std::string>>>,
    schema::Case<"FROMLONLAT", GeoCoordinate,
        schema::Field<&GeoCoordinate::lon, schema::Arg<double>>,
        schema::Field<&GeoCoordinate::lat, schema::Arg<double>>>>;

}  // namespace valkey::commands
