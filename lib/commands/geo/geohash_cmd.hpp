#pragma once
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

struct GeoHash {
    std::string key;
    std::vector<std::string> members;
};

using GeoHashSchema = schema::Schema<GeoHash,
    schema::Bind<&GeoHash::key, schema::Arg<std::string>>,
    schema::Bind<&GeoHash::members, schema::Variadic<std::string, 1>>>;

io::Reply ExecGeoHash(engine::Engine& e, const GeoHash& x) {
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
        result.push_back(io::MakeBulk(structures::geohash::ToBase32(it->second)));
    }
    return io::MakeArray(std::move(result));
}

using GeoHashTrait = CommandTrait<"GEOHASH", GeoHashSchema, &ExecGeoHash>;

}  // namespace valkey::commands
