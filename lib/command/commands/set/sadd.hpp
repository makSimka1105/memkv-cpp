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

struct SAdd {
    std::string key;
    std::vector<std::string> members;
};

using SAddSchema = schema::Schema<SAdd,
    schema::Field<&SAdd::key, schema::Arg<std::string>>,
    schema::Field<&SAdd::members, schema::Variadic<std::string, 1>>>;

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
