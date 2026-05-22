#pragma once
#include <cstdint>
#include <string>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_tree.hpp"
#include "schema/rule.hpp"
#include "schema/schema.hpp"
#include "schema/strategies/arg.hpp"
#include "structures/zset.hpp"

namespace valkey::commands {

struct ZRank {
    std::string key;
    std::string member;
};

using ZRankSchema = schema::Schema<ZRank,
    schema::Bind<&ZRank::key, schema::Arg<std::string>>,
    schema::Bind<&ZRank::member, schema::Arg<std::string>>>;

io::Reply ExecZRank(engine::Engine& e, const ZRank& x) {
    auto [ptr, st] = e.As<structures::ZSet>(x.key);
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
    auto rank = ptr->sl->Rank(it->second, x.member);
    if (!rank) {
        return io::MakeNil();
    }
    return io::MakeInt(static_cast<int64_t>(*rank));
}

using ZRankTrait = CommandTrait<"ZRANK", ZRankSchema, &ExecZRank>;

}  // namespace valkey::commands
