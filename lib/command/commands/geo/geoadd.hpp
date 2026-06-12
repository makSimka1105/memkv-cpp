#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "command/commands/tree.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/schema.hpp"
#include "command/schema/parsers/arg.hpp"
#include "command/schema/combinators/repeat.hpp"
#include "types/all.hpp"

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
    auto status = e.Upsert<types::Geo>(x.key, [&](auto& g) -> std::ptrdiff_t {
        std::ptrdiff_t delta = 0;
        for (const auto& entry : x.entries) {
            if (g.insert_or_assign(entry.member, types::GeoPoint{entry.lon, entry.lat}).second) {
                ++added;
                delta += types::GeoTrait::ElementBytes(entry.member);
            }
        }
        return delta;
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
