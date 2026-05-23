#pragma once
#include <cstdint>
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

namespace valkey::commands {

struct SAdd {
    std::string key;
    std::vector<std::string> members;
};

using SAddSchema = schema::Schema<SAdd,
    schema::Bind<&SAdd::key, schema::Arg<std::string>>,
    schema::Bind<&SAdd::members, schema::Variadic<std::string, 1>>>;

io::Reply ExecSAdd(engine::Engine& e, const SAdd& x) {
    int64_t added = 0;
    auto status = e.Upsert<types::Set>(x.key, [&](auto& s) {
        for (const auto& m : x.members) {
            if (s.insert(m).second) {
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

using SAddTrait = CommandTrait<"SADD", SAddSchema, &ExecSAdd>;

}  // namespace valkey::commands
