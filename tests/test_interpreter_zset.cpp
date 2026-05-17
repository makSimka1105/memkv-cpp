#include <gtest/gtest.h>

#include <memory>
#include <variant>

#include "engine/engine.hpp"
#include "engine/interpreter.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"

using namespace valkey;

static engine::Engine make_eng() { return engine::Engine{}; }

TEST(InterpreterZSet, AddRankRange) {
    auto eng = make_eng();
    auto r = engine::Eval(eng, cmd_nodes::ZAdd{"z", {{1.0, "a"}, {3.0, "c"}, {2.0, "b"}}});
    EXPECT_EQ(std::get<io::Int>(r).value, 3);
    EXPECT_EQ(std::get<io::Int>(engine::Eval(eng, cmd_nodes::ZCard{"z"})).value, 3);
    EXPECT_EQ(std::get<io::Int>(engine::Eval(eng, cmd_nodes::ZRank{"z", "b"})).value, 1);

    auto rng = engine::Eval(eng, cmd_nodes::ZRange{"z", 0, -1, false});
    const auto& arr = std::get<std::unique_ptr<io::Array>>(rng);
    ASSERT_EQ(arr->items.size(), 3u);
    EXPECT_EQ(std::get<io::BulkStr>(arr->items[0]).value, "a");
    EXPECT_EQ(std::get<io::BulkStr>(arr->items[2]).value, "c");
}

TEST(InterpreterZSet, RemAndWrongType) {
    auto eng = make_eng();
    engine::Eval(eng, cmd_nodes::ZAdd{"z", {{1.0, "a"}, {2.0, "b"}}});
    EXPECT_EQ(std::get<io::Int>(engine::Eval(eng, cmd_nodes::ZRem{"z", {"a", "x"}})).value, 1);
    EXPECT_EQ(std::get<io::Int>(engine::Eval(eng, cmd_nodes::ZCard{"z"})).value, 1);

    engine::Eval(eng, cmd_nodes::Set{"k", "val"});
    EXPECT_TRUE(std::holds_alternative<io::Err>(
        engine::Eval(eng, cmd_nodes::ZAdd{"k", {{1.0, "a"}}})));
}
