#pragma once
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "commands/tree.hpp"
#include "schema/combinators/field.hpp"
#include "schema/schema.hpp"
#include "schema/parsers/arg.hpp"
#include "schema/parsers/variadic.hpp"
#include "structures/geohash.hpp"

namespace valkey::commands {

struct GeoHash {
    std::string key;
    std::vector<std::string> members;
};

using GeoHashSchema = schema::Schema<GeoHash,
    schema::Field<&GeoHash::key, schema::Arg<std::string>>,
    schema::Field<&GeoHash::members, schema::Variadic<std::string, 1>>>;

io::Reply ExecGeoHash(engine::Engine& e, const GeoHash& x) {
    auto [ptr, st] = e.As<types::ZSet>(x.key);
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
