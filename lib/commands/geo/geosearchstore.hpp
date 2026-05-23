#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "commands/geo/geo_nodes.hpp"
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"
#include "schema/strategies/choice.hpp"
#include "schema/strategies/keyword.hpp"
#include "schema/strategies/one_of.hpp"
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
    schema::Bind<&GeoSearchStore::from, schema::OneOf<
        schema::KeywordGroup<"FROMMEMBER", GeoFromMember,
            schema::Bind<&GeoFromMember::member, schema::Arg<std::string>>>,
        schema::KeywordGroup<"FROMLONLAT", GeoFromLonLat,
            schema::Bind<&GeoFromLonLat::lon, schema::Arg<double>>,
            schema::Bind<&GeoFromLonLat::lat, schema::Arg<double>>>>>,
    schema::Bind<&GeoSearchStore::by, schema::OneOf<
        schema::KeywordGroup<"BYRADIUS", GeoByRadius,
            schema::Bind<&GeoByRadius::radius, schema::Arg<double>>,
            schema::Bind<&GeoByRadius::unit, schema::Arg<std::string>>>,
        schema::KeywordGroup<"BYBOX", GeoByBox,
            schema::Bind<&GeoByBox::width, schema::Arg<double>>,
            schema::Bind<&GeoByBox::height, schema::Arg<double>>,
            schema::Bind<&GeoByBox::unit, schema::Arg<std::string>>>>>,
    schema::AnyOrder<
        schema::Bind<&GeoSearchStore::sort_dir, schema::OptionalChoice<"ASC", "DESC">>,
        schema::Bind<&GeoSearchStore::count, schema::Keyword<"COUNT", int64_t>>>>;

io::Reply ExecGeoSearchStore(engine::Engine& e, const GeoSearchStore& x) {
    if (!structures::geo::ValidUnit(UnitOf(x.by))) {
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
    auto center = ResolveCenter(x.from, *ptr);
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
