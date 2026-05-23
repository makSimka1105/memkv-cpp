#pragma once

#include <cmath>
#include <numbers>
#include <string_view>

namespace valkey::structures::geo {

static constexpr double kEarthRadius = 6372800.0;  // meters (6372.8 km from spec)

double Haversine(double lon1, double lat1, double lon2, double lat2) {
    auto rad = [](double deg) { return deg * std::numbers::pi / 180.0; };
    double dlat = rad(lat2 - lat1) / 2.0;
    double dlon = rad(lon2 - lon1) / 2.0;
    double a = std::sin(dlat) * std::sin(dlat) +
               std::cos(rad(lat1)) * std::cos(rad(lat2)) * std::sin(dlon) * std::sin(dlon);
    return kEarthRadius * 2.0 * std::asin(std::sqrt(a));
}

double ToMeters(double v, std::string_view unit) {
    if (unit == "km" || unit == "KM") {
        return v * 1000.0;
    }
    if (unit == "mi" || unit == "MI") {
        return v * 1609.344;
    }
    if (unit == "ft" || unit == "FT") {
        return v * 0.3048;
    }
    return v;
}

double ToUnit(double meters, std::string_view unit) {
    if (unit == "km" || unit == "KM") {
        return meters / 1000.0;
    }
    if (unit == "mi" || unit == "MI") {
        return meters / 1609.344;
    }
    if (unit == "ft" || unit == "FT") {
        return meters / 0.3048;
    }
    return meters;
}

bool ValidUnit(std::string_view unit) {
    return unit == "m" || unit == "M" || unit == "km" || unit == "KM" || unit == "mi" ||
           unit == "MI" || unit == "ft" || unit == "FT";
}

}  // namespace valkey::structures::geo
