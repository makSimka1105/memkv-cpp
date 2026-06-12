#pragma once
#include <cstdint>
#include <string>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "command/commands/tree.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/schema.hpp"
#include "command/schema/parsers/arg.hpp"

namespace valkey::commands {

struct LSet {
    std::string key;
    int64_t index;
    std::string value;
};

using LSetSchema = schema::Schema<LSet,
    schema::Field<&LSet::key, schema::Arg<std::string>>,
    schema::Field<&LSet::index, schema::Arg<int64_t>>,
    schema::Field<&LSet::value, schema::Arg<std::string>>>;

io::Reply ExecLSet(engine::Engine& e, const LSet& x) {
    bool out_of_range = false;
    auto st = e.Mutate<types::List>(x.key, [&](auto& lst) {
        const int64_t len = lst.size();
        int64_t idx = x.index < 0 ? len + x.index : x.index;
        if (idx < 0 || idx >= len) {
            out_of_range = true;
            return;
        }
        lst[idx] = x.value;
    });
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeErr("ERR no such key");
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (out_of_range) {
        return io::MakeErr("ERR index out of range");
    }
    return io::Ok();
}

using LSetTrait = CommandTrait<"LSET", LSetSchema, &ExecLSet>;

}  // namespace valkey::commands
