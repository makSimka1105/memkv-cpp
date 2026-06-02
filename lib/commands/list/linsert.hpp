#pragma once
#include <algorithm>
#include <cstdint>
#include <string>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "commands/tree.hpp"
#include "schema/combinators/field.hpp"
#include "schema/schema.hpp"
#include "schema/parsers/arg.hpp"
#include "schema/parsers/choice.hpp"

namespace valkey::commands {

struct LInsert {
    std::string key;
    bool before;
    std::string pivot;
    std::string value;
};

using LInsertSchema = schema::Schema<LInsert,
    schema::Field<&LInsert::key, schema::Arg<std::string>>,
    schema::Field<&LInsert::before, schema::Choice<"BEFORE", "AFTER">>,
    schema::Field<&LInsert::pivot, schema::Arg<std::string>>,
    schema::Field<&LInsert::value, schema::Arg<std::string>>>;

io::Reply ExecLInsert(engine::Engine& e, const LInsert& x) {
    int64_t new_len = -1;
    auto st = e.Mutate<types::List>(x.key, [&](auto& lst) {
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
