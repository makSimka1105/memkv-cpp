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
#include "commands/tree.hpp"
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/overload.hpp"
#include "schema/combinators/any_order.hpp"
#include "schema/combinators/field.hpp"
#include "schema/parsers/arg.hpp"
#include "schema/parsers/flag.hpp"
#include "schema/parsers/keyword.hpp"
#include "schema/parsers/optional_choice.hpp"
#include "schema/schema.hpp"
#include "structures/geo.hpp"

namespace valkey::commands {

struct GeoSearch {
    std::string key;
    GeoFrom::Type from;
    GeoBy::Type by;
    std::optional<bool> sort_dir;
    int64_t count{0};
    bool withcoord{false};
    bool withdist{false};
};

using GeoSearchSchema = schema::Schema<
    GeoSearch,
    schema::Field<&GeoSearch::key, schema::Arg<std::string>>,
    schema::Field<&GeoSearch::from, GeoFrom>,
    schema::Field<&GeoSearch::by, GeoBy>,
    schema::AnyOrder<
        schema::Field<&GeoSearch::sort_dir, schema::OptionalChoice<"ASC", "DESC">>,
        schema::Field<&GeoSearch::count, schema::Keyword<"COUNT", int64_t>>,
        schema::Field<&GeoSearch::withcoord, schema::Flag<"WITHCOORD">>,
        schema::Field<&GeoSearch::withdist, schema::Flag<"WITHDIST">>>>;

io::Reply ExecGeoSearch(engine::Engine& e, const GeoSearch& x) {
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
    auto center = std::visit(Overload{
        [&](const GeoMemberRef& m) { return CenterFromMember(m, *ptr); },
        [&](const GeoCoordinate& p) { return CenterFromCoord(p, *ptr); },
    }, x.from);
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
