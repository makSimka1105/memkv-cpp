#pragma once
#include <cstdint>
#include <string>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "commands/tree.hpp"
#include "schema/combinators/field.hpp"
#include "schema/schema.hpp"
#include "schema/parsers/arg.hpp"

namespace valkey::commands {

struct Append {
    std::string key;
    std::string value;
};

using AppendSchema = schema::Schema<Append,
    schema::Field<&Append::key, schema::Arg<std::string>>,
    schema::Field<&Append::value, schema::Arg<std::string>>>;

io::Reply ExecAppend(engine::Engine& e, const Append& x) {
    int64_t new_len = 0;
    auto status = e.Upsert<std::string>(x.key, [&](std::string& s) {
        s += x.value;
        new_len = s.size();
    });
    if (status == engine::Engine::UpsertStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (status == engine::Engine::UpsertStatus::kOom) {
        return engine::OomReply();
    }
    return io::MakeInt(new_len);
}

using AppendTrait = CommandTrait<"APPEND", AppendSchema, &ExecAppend>;

}  // namespace valkey::commands
