#pragma once

#include <optional>
#include <string>
#include <utility>

#include "command/schema/combinators/field.hpp"
#include "command/schema/combinators/group.hpp"
#include "command/schema/combinators/one_of.hpp"
#include "command/schema/parsers/arg.hpp"
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

GeoCenter CenterFromMember(const GeoMemberRef& m, const types::Geo& g) {
    auto it = g.find(m.member);
    if (it == g.end()) {
        return std::nullopt;
    }
    return std::pair{it->second.lon, it->second.lat};
}

GeoCenter CenterFromCoord(const GeoCoordinate& p, const types::Geo&) {
    return std::pair{p.lon, p.lat};
}

using GeoFrom = schema::OneOf<
    schema::Case<"FROMMEMBER", GeoMemberRef,
        schema::Field<&GeoMemberRef::member, schema::Arg<std::string>>>,
    schema::Case<"FROMLONLAT", GeoCoordinate,
        schema::Field<&GeoCoordinate::lon, schema::Arg<double>>,
        schema::Field<&GeoCoordinate::lat, schema::Arg<double>>>>;

}  // namespace valkey::commands
