#pragma once

#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

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
#include "schema/strategies/flag.hpp"
#include "schema/strategies/keyword.hpp"
#include "schema/strategies/switch_on.hpp"
#include "structures/geo.hpp"

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
    schema::Bind<&GeoSearch::from, schema::SwitchOn<
        AltCase<"FROMMEMBER", GeoMemberRef, &CenterFromMember,
            schema::Bind<&GeoMemberRef::member, schema::Arg<std::string>>>,
        AltCase<"FROMLONLAT", GeoCoordinate, &CenterFromCoord,
            schema::Bind<&GeoCoordinate::lon, schema::Arg<double>>,
            schema::Bind<&GeoCoordinate::lat, schema::Arg<double>>>>>,
    schema::Bind<&GeoSearch::by, schema::SwitchOn<
        AltCase<"BYRADIUS", GeoRadiusFilter, &RadiusContains,
            schema::Bind<&GeoRadiusFilter::radius, schema::Arg<double>>,
            schema::Bind<&GeoRadiusFilter::unit, schema::Arg<std::string>>>,
        AltCase<"BYBOX", GeoBoxFilter, &BoxContains,
            schema::Bind<&GeoBoxFilter::width, schema::Arg<double>>,
            schema::Bind<&GeoBoxFilter::height, schema::Arg<double>>,
            schema::Bind<&GeoBoxFilter::unit, schema::Arg<std::string>>>>>,
    schema::AnyOrder<
        schema::Bind<&GeoSearch::sort_dir, schema::OptionalChoice<"ASC", "DESC">>,
        schema::Bind<&GeoSearch::count, schema::Keyword<"COUNT", int64_t>>,
        schema::Bind<&GeoSearch::withcoord, schema::Flag<"WITHCOORD">>,
        schema::Bind<&GeoSearch::withdist, schema::Flag<"WITHDIST">>>>;

inline io::Reply ExecGeoSearch(engine::Engine& e, const GeoSearch& x) {
    auto unit = std::visit([](const auto& by) -> std::string_view { return by.unit; }, x.by);
    if (!structures::geo::ValidUnit(unit)) {
        return io::MakeErr("unsupported unit provided. please use M, KM, FT, MI");
    }
    auto [ptr, st] = e.As<types::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
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
