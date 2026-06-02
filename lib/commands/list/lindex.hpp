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

struct LIndex {
    std::string key;
    int64_t index;
};

using LIndexSchema = schema::Schema<LIndex,
    schema::Field<&LIndex::key, schema::Arg<std::string>>,
    schema::Field<&LIndex::index, schema::Arg<int64_t>>>;

io::Reply ExecLIndex(engine::Engine& e, const LIndex& x) {
    auto [lst, st] = e.As<types::List>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeNil();
    }
    const int64_t len = lst->size();
    int64_t idx = x.index < 0 ? len + x.index : x.index;
    if (idx < 0 || idx >= len) {
        return io::MakeNil();
    }
    return io::MakeBulk((*lst)[idx]);
}

using LIndexTrait = CommandTrait<"LINDEX", LIndexSchema, &ExecLIndex>;

}  // namespace valkey::commands
