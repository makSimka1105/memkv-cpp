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
#include "types/all.hpp"

namespace valkey::commands {

struct ZAddEntry {
    double score;
    std::string member;
};

struct ZAdd {
    std::string key;
    std::vector<ZAddEntry> entries;
};

using ZAddSchema = schema::Schema<ZAdd,
    schema::Field<&ZAdd::key, schema::Arg<std::string>>,
    schema::Field<&ZAdd::entries, schema::Repeat<ZAddEntry,
        schema::Field<&ZAddEntry::score, schema::Arg<double>>,
        schema::Field<&ZAddEntry::member, schema::Arg<std::string>>>>>;

io::Reply ExecZAdd(engine::Engine& e, const ZAdd& x) {
    if (x.entries.empty()) {
        return io::MakeErr("ERR wrong number of arguments for 'zadd' command");
    }
    int64_t added = 0;
    auto status = e.Upsert<types::ZSet>(x.key, [&](auto& z) {
        for (const auto& entry : x.entries) {
            if (z.SetScore(entry.member, entry.score)) {
                ++added;
            }
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

using ZAddTrait = CommandTrait<"ZADD", ZAddSchema, &ExecZAdd>;

}  // namespace valkey::commands
