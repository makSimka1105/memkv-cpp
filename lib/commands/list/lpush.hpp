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
#include "schema/parsers/variadic.hpp"

namespace valkey::commands {

struct LPush {
    std::string key;
    std::vector<std::string> values;
};

using LPushSchema = schema::Schema<LPush,
    schema::Field<&LPush::key, schema::Arg<std::string>>,
    schema::Field<&LPush::values, schema::Variadic<std::string, 1>>>;

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
