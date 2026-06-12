#pragma once
#include <string>
#include <string_view>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "command/commands/tree.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/schema.hpp"
#include "command/schema/parsers/arg.hpp"
#include "command/schema/parsers/optional.hpp"
#include "structures/geo.hpp"
#include "types/all.hpp"

namespace valkey::commands {

struct GeoDist {
    std::string key;
    std::string member1;
    std::string member2;
    std::string unit;
};

using GeoDistSchema = schema::Schema<GeoDist,
    schema::Field<&GeoDist::key, schema::Arg<std::string>>,
    schema::Field<&GeoDist::member1, schema::Arg<std::string>>,
    schema::Field<&GeoDist::member2, schema::Arg<std::string>>,
    schema::Field<&GeoDist::unit, schema::Optional<std::string>>>;

io::Reply ExecGeoDist(engine::Engine& e, const GeoDist& x) {
    std::string_view unit = x.unit.empty() ? "m" : std::string_view{x.unit};
    if (!structures::geo::ValidUnit(unit)) {
        return io::MakeErr("unsupported unit provided. please use M, KM, FT, MI");
    }

    auto [ptr, st] = e.As<types::Geo>(x.key);
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeNil();
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }

    auto it1 = ptr->find(x.member1);
    auto it2 = ptr->find(x.member2);
    if (it1 == ptr->end() || it2 == ptr->end()) {
        return io::MakeNil();
    }

    const auto& [lon1, lat1] = it1->second;
    const auto& [lon2, lat2] = it2->second;
    double meters = structures::geo::Haversine(lon1, lat1, lon2, lat2);
    double dist = structures::geo::ToUnit(meters, unit);
    return io::MakeBulk(dist);
}

using GeoDistTrait = CommandTrait<"GEODIST", GeoDistSchema, &ExecGeoDist>;

}  // namespace valkey::commands
