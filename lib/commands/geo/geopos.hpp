#pragma once
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
#include "schema/strategies/variadic.hpp"
#include "structures/geohash.hpp"
#include "structures/zset.hpp"

namespace valkey::commands {

struct GeoPos {
    std::string key;
    std::vector<std::string> members;
};

using GeoPosSchema = schema::Schema<GeoPos,
    schema::Bind<&GeoPos::key, schema::Arg<std::string>>,
    schema::Bind<&GeoPos::members, schema::Variadic<std::string, 1>>>;

io::Reply ExecGeoPos(engine::Engine& e, const GeoPos& x) {
    auto [ptr, st] = e.As<structures::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }

    std::vector<io::Reply> result;
    result.reserve(x.members.size());
    for (const auto& m : x.members) {
        if (st == engine::Engine::TypeStatus::kMissing) {
            result.push_back(io::MakeNil());
            continue;
        }
        auto it = ptr->scores.find(m);
        if (it == ptr->scores.end()) {
            result.push_back(io::MakeNil());
            continue;
        }
        auto [lon, lat] = structures::geohash::Decode(it->second);
        std::vector<io::Reply> pair;
        pair.push_back(io::MakeBulk(std::format("{:.17g}", lon)));
        pair.push_back(io::MakeBulk(std::format("{:.17g}", lat)));
        result.push_back(io::MakeArray(std::move(pair)));
    }
    return io::MakeArray(std::move(result));
}

using GeoPosTrait = CommandTrait<"GEOPOS", GeoPosSchema, &ExecGeoPos>;

}  // namespace valkey::commands
