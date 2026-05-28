#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "schema/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"
#include "schema/strategies/repeat.hpp"

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
    schema::Bind<&ZAdd::key, schema::Arg<std::string>>,
    schema::Bind<&ZAdd::entries, schema::RepeatValues<ZAddEntry,
        schema::Bind<&ZAddEntry::score, schema::Arg<double>>,
        schema::Bind<&ZAddEntry::member, schema::Arg<std::string>>>>>;

io::Reply ExecZAdd(engine::Engine& e, const ZAdd& x) {
    if (x.entries.empty()) {
        return io::MakeErr("ERR wrong number of arguments for 'zadd' command");
    }
    int64_t added = 0;
    auto status = e.Upsert<types::ZSet>(x.key, [&](auto& z) {
        for (const auto& entry : x.entries) {
            auto it = z.scores.find(entry.member);
            if (it != z.scores.end()) {
                z.sl->Erase(it->second, entry.member);
                it->second = entry.score;
            } else {
                z.scores[entry.member] = entry.score;
                ++added;
            }
            z.sl->Insert(entry.score, entry.member);
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
