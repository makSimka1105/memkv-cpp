#pragma once
#include <format>
#include <optional>
#include <string>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"
#include "structures/geo.hpp"
#include "structures/geohash.hpp"

namespace valkey::commands {

struct GeoDist {
    std::string key;
    std::string member1;
    std::string member2;
    std::string unit;
};

using GeoDistSchema = schema::Schema<GeoDist,
    schema::Bind<&GeoDist::key, schema::Arg<std::string>>,
    schema::Bind<&GeoDist::member1, schema::Arg<std::string>>,
    schema::Bind<&GeoDist::member2, schema::Arg<std::string>>,
    schema::Bind<&GeoDist::unit, schema::OptArg<std::string>>>;

io::Reply ExecGeoDist(engine::Engine& e, const GeoDist& x) {
    std::string_view unit = x.unit.empty() ? "m" : std::string_view{x.unit};
    if (!structures::geo::ValidUnit(unit)) {
        return io::MakeErr("unsupported unit provided. please use M, KM, FT, MI");
    }

    auto [ptr, st] = e.As<types::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeNil();
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }

    auto it1 = ptr->scores.find(x.member1);
    auto it2 = ptr->scores.find(x.member2);
    if (it1 == ptr->scores.end() || it2 == ptr->scores.end()) {
        return io::MakeNil();
    }

    auto [lon1, lat1] = structures::geohash::Decode(it1->second);
    auto [lon2, lat2] = structures::geohash::Decode(it2->second);
    double meters = structures::geo::Haversine(lon1, lat1, lon2, lat2);
    double dist = structures::geo::ToUnit(meters, unit);
    return io::MakeBulk(std::format("{:.4f}", dist));
}

using GeoDistTrait = CommandTrait<"GEODIST", GeoDistSchema, &ExecGeoDist>;

}  // namespace valkey::commands
