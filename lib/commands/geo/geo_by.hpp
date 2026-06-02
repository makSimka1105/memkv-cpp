#pragma once

#include <string>

#include "schema/combinators/field.hpp"
#include "schema/combinators/group.hpp"
#include "schema/combinators/one_of.hpp"
#include "schema/parsers/arg.hpp"
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

bool RadiusContains(const GeoRadiusFilter& r,
                           double, double, double, double, double dist_m) {
    return dist_m <= structures::geo::ToMeters(r.radius, r.unit);
}

bool BoxContains(const GeoBoxFilter& b,
                        double clon, double clat, double mlon, double mlat, double) {
    double hw = structures::geo::ToMeters(b.width, b.unit) / 2.0;
    double hh = structures::geo::ToMeters(b.height, b.unit) / 2.0;
    double dx = structures::geo::Haversine(clon, clat, mlon, clat);
    double dy = structures::geo::Haversine(clon, clat, clon, mlat);
    return dx <= hw && dy <= hh;
}

using GeoBy = schema::OneOf<
    schema::Case<"BYRADIUS", GeoRadiusFilter,
        schema::Field<&GeoRadiusFilter::radius, schema::Arg<double>>,
        schema::Field<&GeoRadiusFilter::unit, schema::Arg<std::string>>>,
    schema::Case<"BYBOX", GeoBoxFilter,
        schema::Field<&GeoBoxFilter::width, schema::Arg<double>>,
        schema::Field<&GeoBoxFilter::height, schema::Arg<double>>,
        schema::Field<&GeoBoxFilter::unit, schema::Arg<std::string>>>>;

}  // namespace valkey::commands
