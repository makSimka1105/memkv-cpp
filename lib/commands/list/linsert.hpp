#pragma once
#include <algorithm>
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
#include "schema/strategies/choice.hpp"

namespace valkey::commands {

struct LInsert {
    std::string key;
    bool before;
    std::string pivot;
    std::string value;
};

using LInsertSchema = schema::Schema<LInsert,
    schema::Bind<&LInsert::key, schema::Arg<std::string>>,
    schema::Bind<&LInsert::before, schema::Choice<"BEFORE", "AFTER">>,
    schema::Bind<&LInsert::pivot, schema::Arg<std::string>>,
    schema::Bind<&LInsert::value, schema::Arg<std::string>>>;

io::Reply ExecLInsert(engine::Engine& e, const LInsert& x) {
    int64_t new_len = -1;
    auto st = e.Mutate<std::deque<std::string>>(x.key, [&](auto& lst) {
        auto it = std::find(lst.begin(), lst.end(), x.pivot);
        if (it == lst.end()) {
            return;
        }
        if (!x.before) {
            ++it;
        }
        lst.insert(it, x.value);
        new_len = lst.size();
    });
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    return io::MakeInt(new_len);
}

using LInsertTrait = CommandTrait<"LINSERT", LInsertSchema, &ExecLInsert>;

}  // namespace valkey::commands
