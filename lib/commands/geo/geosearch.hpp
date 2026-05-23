#pragma once
#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "commands/geo/geo_nodes.hpp"
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"
#include "schema/strategies/choice.hpp"
#include "schema/strategies/flag.hpp"
#include "schema/strategies/keyword.hpp"
#include "schema/strategies/one_of.hpp"
#include "structures/geo.hpp"
#include "structures/zset.hpp"

namespace valkey::commands {

struct GeoSearch {
    std::string key;
    GeoFrom from;
    GeoBy by;
    std::optional<bool> sort_dir;
    int64_t count{0};
    bool withcoord{false};
    bool withdist{false};
};

using GeoSearchSchema = schema::Schema<
    GeoSearch,
    schema::Bind<&GeoSearch::key, schema::Arg<std::string>>,
    schema::Bind<&GeoSearch::from, schema::OneOf<
        schema::KeywordGroup<"FROMMEMBER", GeoFromMember,
            schema::Bind<&GeoFromMember::member, schema::Arg<std::string>>>,
        schema::KeywordGroup<"FROMLONLAT", GeoFromLonLat,
            schema::Bind<&GeoFromLonLat::lon, schema::Arg<double>>,
            schema::Bind<&GeoFromLonLat::lat, schema::Arg<double>>>>>,
    schema::Bind<&GeoSearch::by, schema::OneOf<
        schema::KeywordGroup<"BYRADIUS", GeoByRadius,
            schema::Bind<&GeoByRadius::radius, schema::Arg<double>>,
            schema::Bind<&GeoByRadius::unit, schema::Arg<std::string>>>,
        schema::KeywordGroup<"BYBOX", GeoByBox,
            schema::Bind<&GeoByBox::width, schema::Arg<double>>,
            schema::Bind<&GeoByBox::height, schema::Arg<double>>,
            schema::Bind<&GeoByBox::unit, schema::Arg<std::string>>>>>,
    schema::AnyOrder<
        schema::Bind<&GeoSearch::sort_dir, schema::OptionalChoice<"ASC", "DESC">>,
        schema::Bind<&GeoSearch::count, schema::Keyword<"COUNT", int64_t>>,
        schema::Bind<&GeoSearch::withcoord, schema::Flag<"WITHCOORD">>,
        schema::Bind<&GeoSearch::withdist, schema::Flag<"WITHDIST">>>>;

io::Reply ExecGeoSearch(engine::Engine& e, const GeoSearch& x) {
    if (!structures::geo::ValidUnit(UnitOf(x.by))) {
        return io::MakeErr("unsupported unit provided. please use M, KM, FT, MI");
    }
    auto [ptr, st] = e.As<structures::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
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

    std::string_view unit = UnitOf(x.by);
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
            row.push_back(io::MakeBulk(std::format("{:.4f}", structures::geo::ToUnit(h.dist_m, unit))));
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
