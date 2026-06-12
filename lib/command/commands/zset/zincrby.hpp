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

struct ZIncrBy {
    std::string key;
    double increment;
    std::string member;
};

using ZIncrBySchema = schema::Schema<ZIncrBy,
    schema::Field<&ZIncrBy::key, schema::Arg<std::string>>,
    schema::Field<&ZIncrBy::increment, schema::Arg<double>>,
    schema::Field<&ZIncrBy::member, schema::Arg<std::string>>>;

io::Reply ExecZIncrBy(engine::Engine& e, const ZIncrBy& x) {
    double new_score = 0.0;
    auto status = e.Upsert<types::ZSet>(x.key, [&](auto& z) {
        auto it = z.scores.find(x.member);
        new_score = (it != z.scores.end() ? it->second : 0.0) + x.increment;
        z.SetScore(x.member, new_score);
    });
    if (status == engine::Engine::UpsertStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (status == engine::Engine::UpsertStatus::kOom) {
        return engine::OomReply();
    }
    return io::MakeBulk(std::format("{:.17g}", new_score));
}

using ZIncrByTrait = CommandTrait<"ZINCRBY", ZIncrBySchema, &ExecZIncrBy>;

}  // namespace valkey::commands
