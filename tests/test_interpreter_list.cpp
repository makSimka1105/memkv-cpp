#include <cstdint>
#include <memory>
#include <variant>

#include <gtest/gtest.h>

#include "commands/lindex.hpp"
#include "commands/linsert.hpp"
#include "commands/llen.hpp"
#include "commands/lpop.hpp"
#include "commands/lpush.hpp"
#include "commands/lrange.hpp"
#include "commands/lset.hpp"
#include "commands/rpop.hpp"
#include "commands/rpush.hpp"
#include "commands/set.hpp"
#include "engine/engine.hpp"
#include "engine/interpreter.hpp"
#include "io/reply.hpp"
#include "commands/all.hpp"

using namespace valkey;

TEST(InterpreterList, LPushAndLRange) {
    auto eng = engine::Engine{};
    auto r = engine::Eval(eng, cmd_nodes::LPush{"k", {"a", "b", "c"}});
    ASSERT_TRUE(std::holds_alternative<io::Int>(r));
    EXPECT_EQ(std::get<io::Int>(r).value, 3);

    auto r2 = engine::Eval(eng, cmd_nodes::LRange{"k", 0, -1});
    const auto* arr = std::get_if<std::unique_ptr<io::Array>>(&r2);
    ASSERT_NE(arr, nullptr);
    ASSERT_EQ((*arr)->items.size(), 3u);
    EXPECT_EQ(std::get<io::BulkStr>((*arr)->items[0]).value, "c");
    EXPECT_EQ(std::get<io::BulkStr>((*arr)->items[2]).value, "a");
}

TEST(InterpreterList, LPopRPopWithCount) {
    auto eng = engine::Engine{};
    engine::Eval(eng, cmd_nodes::RPush{"k", {"a", "b", "c"}});

    auto lp = engine::Eval(eng, cmd_nodes::LPop{"k", int64_t{2}});
    const auto& la = std::get<std::unique_ptr<io::Array>>(lp);
    ASSERT_EQ(la->items.size(), 2u);
    EXPECT_EQ(std::get<io::BulkStr>(la->items[0]).value, "a");

    engine::Eval(eng, cmd_nodes::RPush{"k2", {"x", "y", "z"}});
    auto rp = engine::Eval(eng, cmd_nodes::RPop{"k2", int64_t{2}});
    const auto& ra = std::get<std::unique_ptr<io::Array>>(rp);
    ASSERT_EQ(ra->items.size(), 2u);
    EXPECT_EQ(std::get<io::BulkStr>(ra->items[0]).value, "z");
}

TEST(InterpreterList, LInsertAndLSet) {
    auto eng = engine::Engine{};
    engine::Eval(eng, cmd_nodes::RPush{"k", {"a", "c"}});

    auto r = engine::Eval(eng, cmd_nodes::LInsert{"k", true, "c", "b"});
    EXPECT_EQ(std::get<io::Int>(r).value, 3);

    engine::Eval(eng, cmd_nodes::LSet{"k", 1, "B"});
    auto idx = engine::Eval(eng, cmd_nodes::LIndex{"k", 1});
    EXPECT_EQ(std::get<io::BulkStr>(idx).value, "B");
}

TEST(InterpreterList, WrongType) {
    auto eng = engine::Engine{};
    engine::Eval(eng, cmd_nodes::Set{"k", "hello"});
    EXPECT_TRUE(std::holds_alternative<io::Err>(engine::Eval(eng, cmd_nodes::LLen{"k"})));
    EXPECT_TRUE(std::holds_alternative<io::Err>(engine::Eval(eng, cmd_nodes::LPush{"k", {"a"}})));
}
