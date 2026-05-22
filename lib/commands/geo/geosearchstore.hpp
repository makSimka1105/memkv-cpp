#pragma once
#include <algorithm>
#include <cstdint>
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

struct GeoSearchStore {
    std::string dest;
    std::string src;
    double lon;
    double lat;
    double radius;
    std::string unit;
    bool asc;
    bool desc;
    int64_t count;
};

using GeoSearchStoreSchema = schema::Schema<GeoSearchStore,
    schema::Bind<&GeoSearchStore::dest, schema::Arg<std::string>>,
    schema::Bind<&GeoSearchStore::src, schema::Arg<std::string>>,
    schema::LiteralRule<"FROMLONLAT">,
    schema::Bind<&GeoSearchStore::lon, schema::Arg<double>>,
    schema::Bind<&GeoSearchStore::lat, schema::Arg<double>>,
    schema::LiteralRule<"BYRADIUS">,
    schema::Bind<&GeoSearchStore::radius, schema::Arg<double>>,
    schema::Bind<&GeoSearchStore::unit, schema::Arg<std::string>>,
    schema::AnyOrder<
        schema::Bind<&GeoSearchStore::asc, schema::Flag<"ASC">>,
        schema::Bind<&GeoSearchStore::desc, schema::Flag<"DESC">>,
        schema::Bind<&GeoSearchStore::count, schema::Keyword<"COUNT", int64_t>>
    >>;

namespace {

double GssToMeters(double v, std::string_view unit) {
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

io::Reply ExecGeoSearchStore(engine::Engine& e, const GeoSearchStore& x) {
    if (!structures::geo::ValidUnit(x.unit)) {
        return io::MakeErr("unsupported unit provided. please use M, KM, FT, MI");
    }

    auto [ptr, st] = e.As<structures::ZSet>(x.src);
    if (st == engine::Engine::TypeStatus::kMissing) {
        e.DelKey(x.dest);
        return io::MakeInt(0);
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }

    double r_m = GssToMeters(x.radius, x.unit);

    struct Hit {
        std::string member;
        double dist_m;
        double score;
    };

    std::vector<Hit> hits;
    for (const auto& [member, score] : ptr->scores) {
        auto [mlon, mlat] = structures::geohash::Decode(score);
        double d = structures::geo::Haversine(x.lon, x.lat, mlon, mlat);
        if (d <= r_m) {
            hits.push_back({member, d, score});
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

    int64_t stored = static_cast<int64_t>(hits.size());
    if (hits.empty()) {
        e.DelKey(x.dest);
        return io::MakeInt(0);
    }

    auto status = e.Upsert<structures::ZSet>(x.dest, [&](auto& z) {
        z.scores.clear();
        // Clear old skip list by rebuilding
        z.sl = std::make_unique<structures::SkipList<double, std::string>>();
        for (const auto& h : hits) {
            z.scores[h.member] = h.score;
            z.sl->Insert(h.score, h.member);
        }
    });
    if (status == engine::Engine::UpsertStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (status == engine::Engine::UpsertStatus::kOom) {
        return engine::OomReply();
    }
    return io::MakeInt(stored);
}

using GeoSearchStoreTrait = CommandTrait<"GEOSEARCHSTORE", GeoSearchStoreSchema, &ExecGeoSearchStore>;

}  // namespace valkey::commands
