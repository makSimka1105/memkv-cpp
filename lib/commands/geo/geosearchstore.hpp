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
#include "commands/tree.hpp"
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/overload.hpp"
#include "schema/combinators/any_order.hpp"
#include "schema/combinators/field.hpp"
#include "schema/parsers/arg.hpp"
#include "schema/parsers/keyword.hpp"
#include "schema/parsers/optional_choice.hpp"
#include "schema/schema.hpp"
#include "structures/geo.hpp"
#include "structures/skiplist.hpp"
#include "types/all.hpp"

namespace valkey::commands {

struct GeoSearchStore {
    std::string dest;
    std::string src;
    GeoFrom::Type from;
    GeoBy::Type by;
    std::optional<bool> sort_dir;
    int64_t count{0};
};

using GeoSearchStoreSchema = schema::Schema<
    GeoSearchStore,
    schema::Field<&GeoSearchStore::dest, schema::Arg<std::string>>,
    schema::Field<&GeoSearchStore::src, schema::Arg<std::string>>,
    schema::Field<&GeoSearchStore::from, GeoFrom>,
    schema::Field<&GeoSearchStore::by, GeoBy>,
    schema::AnyOrder<
        schema::Field<&GeoSearchStore::sort_dir, schema::OptionalChoice<"ASC", "DESC">>,
        schema::Field<&GeoSearchStore::count, schema::Keyword<"COUNT", int64_t>>>>;

io::Reply ExecGeoSearchStore(engine::Engine& e, const GeoSearchStore& x) {
    auto unit = std::visit([](const auto& by) -> std::string_view { return by.unit; }, x.by);
    if (!structures::geo::ValidUnit(unit)) {
        return io::MakeErr("unsupported unit. please use M, KM, FT, MI");
    }
    auto [ptr, st] = e.As<types::ZSet>(x.src);
    if (st == engine::Engine::TypeStatus::kMissing) {
        e.DelKey(x.dest);
        return io::MakeInt(0);
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    auto center = std::visit(Overload{
        [&](const GeoMemberRef& m) { return CenterFromMember(m, *ptr); },
        [&](const GeoCoordinate& p) { return CenterFromCoord(p, *ptr); },
    }, x.from);
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
