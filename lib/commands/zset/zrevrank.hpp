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

struct ZRevRank {
    std::string key;
    std::string member;
};

using ZRevRankSchema = schema::Schema<ZRevRank,
    schema::Field<&ZRevRank::key, schema::Arg<std::string>>,
    schema::Field<&ZRevRank::member, schema::Arg<std::string>>>;

io::Reply ExecZRevRank(engine::Engine& e, const ZRevRank& x) {
    auto [ptr, st] = e.As<types::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeNil();
    }
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    auto it = ptr->scores.find(x.member);
    if (it == ptr->scores.end()) {
        return io::MakeNil();
    }
    auto rank = ptr->sl->RankReverse(it->second, x.member);
    if (!rank) {
        return io::MakeNil();
    }
    return io::MakeInt(static_cast<int64_t>(*rank));
}

using ZRevRankTrait = CommandTrait<"ZREVRANK", ZRevRankSchema, &ExecZRevRank>;

}  // namespace valkey::commands
