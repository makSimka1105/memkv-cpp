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
#include "schema/strategies/variadic.hpp"

namespace valkey::commands {

struct LPush {
    std::string key;
    std::vector<std::string> values;
};

using LPushSchema = schema::Schema<LPush,
    schema::Bind<&LPush::key, schema::Arg<std::string>>,
    schema::Bind<&LPush::values, schema::Variadic<std::string, 1>>>;

io::Reply ExecLPush(engine::Engine& e, const LPush& x) {
    int64_t len = 0;
    auto status = e.Upsert<types::List>(x.key, [&](auto& lst) {
        for (const auto& v : x.values) {
            lst.push_front(v);
        }
        len = lst.size();
    });
    if (status == engine::Engine::UpsertStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (status == engine::Engine::UpsertStatus::kOom) {
        return engine::OomReply();
    }
    return io::MakeInt(len);
}

using LPushTrait = CommandTrait<"LPUSH", LPushSchema, &ExecLPush>;

}  // namespace valkey::commands
