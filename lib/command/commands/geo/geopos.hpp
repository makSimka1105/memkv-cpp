#pragma once
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "command/commands/tree.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/schema.hpp"
#include "command/schema/parsers/arg.hpp"
#include "command/schema/parsers/variadic.hpp"
#include "types/all.hpp"

namespace valkey::commands {

struct GeoPos {
    std::string key;
    std::vector<std::string> members;
};

using GeoPosSchema = schema::Schema<GeoPos,
    schema::Field<&GeoPos::key, schema::Arg<std::string>>,
    schema::Field<&GeoPos::members, schema::Variadic<std::string, 1>>>;

io::Reply ExecGeoPos(engine::Engine& e, const GeoPos& x) {
    auto [ptr, st] = e.As<types::Geo>(x.key);
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
        auto it = ptr->find(m);
        if (it == ptr->end()) {
            result.push_back(io::MakeNil());
            continue;
        }
        std::vector<io::Reply> pair;
        pair.push_back(io::MakeBulk(it->second.lon));
        pair.push_back(io::MakeBulk(it->second.lat));
        result.push_back(io::MakeArray(std::move(pair)));
    }
    return io::MakeArray(std::move(result));
}

using GeoPosTrait = CommandTrait<"GEOPOS", GeoPosSchema, &ExecGeoPos>;

}  // namespace valkey::commands
