#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "commands/tree.hpp"
#include "schema/combinators/field.hpp"
#include "schema/schema.hpp"
#include "schema/parsers/arg.hpp"
#include "schema/combinators/repeat.hpp"
#include "structures/geohash.hpp"

namespace valkey::commands {

struct GeoAddEntry {
    double lon;
    double lat;
    std::string member;
};

struct GeoAdd {
    std::string key;
    std::vector<GeoAddEntry> entries;
};

using GeoAddSchema = schema::Schema<GeoAdd,
    schema::Field<&GeoAdd::key, schema::Arg<std::string>>,
    schema::Field<&GeoAdd::entries, schema::Repeat<GeoAddEntry,
        schema::Field<&GeoAddEntry::lon, schema::Arg<double>>,
        schema::Field<&GeoAddEntry::lat, schema::Arg<double>>,
        schema::Field<&GeoAddEntry::member, schema::Arg<std::string>>>>>;

io::Reply ExecGeoAdd(engine::Engine& e, const GeoAdd& x) {
    if (x.entries.empty()) {
        return io::MakeErr("ERR wrong number of arguments for 'geoadd' command");
    }
    int64_t added = 0;
    auto status = e.Upsert<types::ZSet>(x.key, [&](auto& z) {
        for (const auto& entry : x.entries) {
            double score = structures::geohash::Encode(entry.lon, entry.lat);
            auto it = z.scores.find(entry.member);
            if (it != z.scores.end()) {
                z.sl->Erase(it->second, entry.member);
                it->second = score;
            } else {
                z.scores[entry.member] = score;
                ++added;
            }
            z.sl->Insert(score, entry.member);
        }
    });
    if (status == engine::Engine::UpsertStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (status == engine::Engine::UpsertStatus::kOom) {
        return engine::OomReply();
    }
    return io::MakeInt(added);
}

using GeoAddTrait = CommandTrait<"GEOADD", GeoAddSchema, &ExecGeoAdd>;

}  // namespace valkey::commands
