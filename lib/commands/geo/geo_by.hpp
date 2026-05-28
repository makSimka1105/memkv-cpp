#pragma once

#include <string>

#include "schema/alt_group.hpp"
#include "schema/rule.hpp"
#include "schema/strategies/arg.hpp"
#include "structures/geo.hpp"

namespace valkey::commands {

struct GeoRadiusFilter {
    double radius{0.0};
    std::string unit;
};

struct GeoBoxFilter {
    double width{0.0};
    double height{0.0};
    std::string unit;
};

inline bool RadiusContains(const GeoRadiusFilter& r,
                           double, double, double, double, double dist_m) {
    return dist_m <= structures::geo::ToMeters(r.radius, r.unit);
}

inline bool BoxContains(const GeoBoxFilter& b,
                        double clon, double clat, double mlon, double mlat, double) {
    double hw = structures::geo::ToMeters(b.width, b.unit) / 2.0;
    double hh = structures::geo::ToMeters(b.height, b.unit) / 2.0;
    double dx = structures::geo::Haversine(clon, clat, mlon, clat);
    double dy = structures::geo::Haversine(clon, clat, clon, mlat);
    return dx <= hw && dy <= hh;
}

using GeoByGroup = AltGroup<
    AltCase<"BYRADIUS", GeoRadiusFilter, &RadiusContains,
        schema::Bind<&GeoRadiusFilter::radius, schema::Arg<double>>,
        schema::Bind<&GeoRadiusFilter::unit, schema::Arg<std::string>>>,
    AltCase<"BYBOX", GeoBoxFilter, &BoxContains,
        schema::Bind<&GeoBoxFilter::width, schema::Arg<double>>,
        schema::Bind<&GeoBoxFilter::height, schema::Arg<double>>,
        schema::Bind<&GeoBoxFilter::unit, schema::Arg<std::string>>>>;

using GeoBy = GeoByGroup::Variant;

}  // namespace valkey::commands
