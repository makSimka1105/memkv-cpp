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
    auto status = e.Upsert<types::ZSet>(x.key, [&](auto& z) -> std::ptrdiff_t {
        std::ptrdiff_t delta = 0;
        for (const auto& entry : x.entries) {
            if (z.SetScore(entry.member, entry.score)) {
                ++added;
                delta += types::ZSetTrait::ElementBytes(entry.member);
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

using ZAddTrait = CommandTrait<"ZADD", ZAddSchema, &ExecZAdd>;

}  // namespace valkey::commands
