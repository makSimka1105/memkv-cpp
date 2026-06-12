#pragma once
#include <format>
#include <string>
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "command/commands/tree.hpp"
#include "command/schema/combinators/field.hpp"
#include "command/schema/schema.hpp"
#include "command/schema/parsers/arg.hpp"

namespace valkey::commands {

struct ZScore {
    std::string key;
    std::string member;
};

using ZScoreSchema = schema::Schema<ZScore,
    schema::Field<&ZScore::key, schema::Arg<std::string>>,
    schema::Field<&ZScore::member, schema::Arg<std::string>>>;

io::Reply ExecZScore(engine::Engine& e, const ZScore& x) {
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
    return io::MakeBulk(std::format("{:.17g}", it->second));
}

using ZScoreTrait = CommandTrait<"ZSCORE", ZScoreSchema, &ExecZScore>;

}  // namespace valkey::commands
