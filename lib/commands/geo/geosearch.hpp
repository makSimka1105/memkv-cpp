#pragma once
#include <algorithm>
#include <cstdint>
#include <format>
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"
#include "schema/strategies/flag.hpp"
#include "schema/strategies/keyword.hpp"
#include "schema/strategies/literal.hpp"
#include "structures/geo.hpp"
#include "structures/geohash.hpp"
#include "structures/zset.hpp"

namespace valkey::commands {

struct GeoSearch {
    std::string key;
    double lon;
    double lat;
    double radius;
    std::string unit;
    bool asc;
    bool desc;
    int64_t count;
    bool withcoord;
    bool withdist;
};

using GeoSearchSchema = schema::Schema<GeoSearch,
    schema::Bind<&GeoSearch::key, schema::Arg<std::string>>,
    schema::LiteralRule<"FROMLONLAT">,
    schema::Bind<&GeoSearch::lon, schema::Arg<double>>,
    schema::Bind<&GeoSearch::lat, schema::Arg<double>>,
    schema::LiteralRule<"BYRADIUS">,
    schema::Bind<&GeoSearch::radius, schema::Arg<double>>,
    schema::Bind<&GeoSearch::unit, schema::Arg<std::string>>,
    schema::AnyOrder<
        schema::Bind<&GeoSearch::asc, schema::Flag<"ASC">>,
        schema::Bind<&GeoSearch::desc, schema::Flag<"DESC">>,
        schema::Bind<&GeoSearch::count, schema::Keyword<"COUNT", int64_t>>,
        schema::Bind<&GeoSearch::withcoord, schema::Flag<"WITHCOORD">>,
        schema::Bind<&GeoSearch::withdist, schema::Flag<"WITHDIST">>
    >>;

namespace {

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

}  // namespace

io::Reply ExecGeoSearch(engine::Engine& e, const GeoSearch& x) {
    if (!structures::geo::ValidUnit(x.unit)) {
        return io::MakeErr("unsupported unit provided. please use M, KM, FT, MI");
    }

    auto [ptr, st] = e.As<structures::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }

    double r_m = ToMeters(x.radius, x.unit);

    struct Hit {
        std::string member;
        double dist_m;
        double lon;
        double lat;
    };

    std::vector<Hit> hits;
    for (const auto& [member, score] : ptr->scores) {
        auto [mlon, mlat] = structures::geohash::Decode(score);
        double d = structures::geo::Haversine(x.lon, x.lat, mlon, mlat);
        if (d <= r_m) {
            hits.push_back({member, d, mlon, mlat});
        }
    }

    if (x.asc) {
        std::ranges::sort(hits, {}, &Hit::dist_m);
    } else if (x.desc) {
        std::ranges::sort(hits, std::ranges::greater{}, &Hit::dist_m);
    }

    if (x.count > 0 && static_cast<size_t>(x.count) < hits.size()) {
        hits.resize(static_cast<size_t>(x.count));
    }

    std::vector<io::Reply> result;
    result.reserve(hits.size());
    for (const auto& h : hits) {
        if (!x.withdist && !x.withcoord) {
            result.push_back(io::MakeBulk(h.member));
            continue;
        }
        std::vector<io::Reply> row;
        row.push_back(io::MakeBulk(h.member));
        if (x.withdist) {
            row.push_back(io::MakeBulk(std::format("{:.4f}", structures::geo::ToUnit(h.dist_m, x.unit))));
        }
        if (x.withcoord) {
            std::vector<io::Reply> coord;
            coord.push_back(io::MakeBulk(std::format("{:.17g}", h.lon)));
            coord.push_back(io::MakeBulk(std::format("{:.17g}", h.lat)));
            row.push_back(io::MakeArray(std::move(coord)));
        }
        result.push_back(io::MakeArray(std::move(row)));
    }
    return io::MakeArray(std::move(result));
}

using GeoSearchTrait = CommandTrait<"GEOSEARCH", GeoSearchSchema, &ExecGeoSearch>;

}  // namespace valkey::commands
