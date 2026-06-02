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

struct ZCard {
    std::string key;
};

using ZCardSchema = schema::Schema<ZCard,
    schema::Field<&ZCard::key, schema::Arg<std::string>>>;

io::Reply ExecZCard(engine::Engine& e, const ZCard& x) {
    auto [ptr, st] = e.As<types::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    return io::MakeInt(static_cast<int64_t>(ptr->sl->Size()));
}

using ZCardTrait = CommandTrait<"ZCARD", ZCardSchema, &ExecZCard>;

}  // namespace valkey::commands
