#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include "commands/geo/geo_by.hpp"
#include "commands/geo/geo_from.hpp"
#include "commands/geo/geo_hit.hpp"
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "schema/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"
#include "schema/strategies/choice.hpp"
#include "schema/strategies/keyword.hpp"
#include "schema/strategies/switch_on.hpp"
#include "structures/geo.hpp"
#include "structures/skiplist.hpp"

namespace valkey::commands {

struct GeoSearchStore {
    std::string dest;
    std::string src;
    GeoFrom from;
    GeoBy by;
    std::optional<bool> sort_dir;
    int64_t count{0};
};

using GeoSearchStoreSchema = schema::Schema<
    GeoSearchStore,
    schema::Bind<&GeoSearchStore::dest, schema::Arg<std::string>>,
    schema::Bind<&GeoSearchStore::src, schema::Arg<std::string>>,
    schema::Bind<&GeoSearchStore::from, schema::SwitchOn<
        AltCase<"FROMMEMBER", GeoMemberRef, &CenterFromMember,
            schema::Bind<&GeoMemberRef::member, schema::Arg<std::string>>>,
        AltCase<"FROMLONLAT", GeoCoordinate, &CenterFromCoord,
            schema::Bind<&GeoCoordinate::lon, schema::Arg<double>>,
            schema::Bind<&GeoCoordinate::lat, schema::Arg<double>>>>>,
    schema::Bind<&GeoSearchStore::by, schema::SwitchOn<
        AltCase<"BYRADIUS", GeoRadiusFilter, &RadiusContains,
            schema::Bind<&GeoRadiusFilter::radius, schema::Arg<double>>,
            schema::Bind<&GeoRadiusFilter::unit, schema::Arg<std::string>>>,
        AltCase<"BYBOX", GeoBoxFilter, &BoxContains,
            schema::Bind<&GeoBoxFilter::width, schema::Arg<double>>,
            schema::Bind<&GeoBoxFilter::height, schema::Arg<double>>,
            schema::Bind<&GeoBoxFilter::unit, schema::Arg<std::string>>>>>,
    schema::AnyOrder<
        schema::Bind<&GeoSearchStore::sort_dir, schema::OptionalChoice<"ASC", "DESC">>,
        schema::Bind<&GeoSearchStore::count, schema::Keyword<"COUNT", int64_t>>>>;

inline io::Reply ExecGeoSearchStore(engine::Engine& e, const GeoSearchStore& x) {
    auto unit = std::visit([](const auto& by) -> std::string_view { return by.unit; }, x.by);
    if (!structures::geo::ValidUnit(unit)) {
        return io::MakeErr("unsupported unit provided. please use M, KM, FT, MI");
    }
    auto [ptr, st] = e.As<types::ZSet>(x.src);
    if (st == engine::Engine::TypeStatus::kMissing) {
        e.DelKey(x.dest);
        return io::MakeInt(0);
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    auto center = GeoFromGroup::Eval(x.from, *ptr);
    if (!center) {
        return io::MakeErr("could not decode requested zset member");
    }
    auto [clon, clat] = *center;
    auto hits = CollectHits(clon, clat, x.by, *ptr, x.sort_dir, x.count);

    int64_t stored = hits.size();
    if (hits.empty()) {
        e.DelKey(x.dest);
        return io::MakeInt(0);
    }
    auto status = e.Upsert<types::ZSet>(x.dest, [&](auto& z) {
        z.scores.clear();
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
