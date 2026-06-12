#pragma once
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
#include "command/schema/parsers/variadic.hpp"

namespace valkey::commands {

struct RPush {
    std::string key;
    std::vector<std::string> values;
};

using RPushSchema = schema::Schema<RPush,
    schema::Field<&RPush::key, schema::Arg<std::string>>,
    schema::Field<&RPush::values, schema::Variadic<std::string, 1>>>;

io::Reply ExecRPush(engine::Engine& e, const RPush& x) {
    int64_t len = 0;
    auto status = e.Upsert<types::List>(x.key, [&](auto& lst) {
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
