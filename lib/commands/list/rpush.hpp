#pragma once
#include <cstdint>
#include <deque>
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

struct RPush {
    std::string key;
    std::vector<std::string> values;
};

using RPushSchema = schema::Schema<RPush,
    schema::Bind<&RPush::key, schema::Arg<std::string>>,
    schema::Bind<&RPush::values, schema::Variadic<std::string, 1>>>;

io::Reply ExecRPush(engine::Engine& e, const RPush& x) {
    int64_t len = 0;
    auto status = e.Upsert<std::deque<std::string>>(x.key, [&](auto& lst) {
        for (const auto& v : x.values) {
            lst.push_back(v);
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

using RPushTrait = CommandTrait<"RPUSH", RPushSchema, &ExecRPush>;

}  // namespace valkey::commands
