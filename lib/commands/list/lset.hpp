#pragma once
#include <cstdint>
#include <deque>
#include <string>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"

namespace valkey::commands {

struct LSet {
    std::string key;
    int64_t index;
    std::string value;
};

using LSetSchema = schema::Schema<LSet,
    schema::Bind<&LSet::key, schema::Arg<std::string>>,
    schema::Bind<&LSet::index, schema::Arg<int64_t>>,
    schema::Bind<&LSet::value, schema::Arg<std::string>>>;

io::Reply ExecLSet(engine::Engine& e, const LSet& x) {
    bool out_of_range = false;
    auto st = e.Mutate<std::deque<std::string>>(x.key, [&](auto& lst) {
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
