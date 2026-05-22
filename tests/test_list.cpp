#include <sstream>
#include <string>
#include <string_view>
#include "engine/engine.hpp"
#include "io/stdin_frontend.hpp"
#include <gtest/gtest.h>

using namespace valkey;

static std::string Exec(engine::Engine& eng, std::string_view cmds) {
    std::istringstream in{std::string(cmds)};
    std::ostringstream out;
    io::RunStdinRepl(eng, in, out);
    return out.str();
}

TEST(ListTest, LPushAndLRange) {
    engine::Engine eng;
    Exec(eng,"LPUSH mylist a b c\n");
    std::string out = Exec(eng,"LRANGE mylist 0 -1\n");
    EXPECT_EQ(out, "1) \"c\"\n2) \"b\"\n3) \"a\"\n");
}

TEST(ListTest, RPushAndLRange) {
    engine::Engine eng;
    Exec(eng,"RPUSH mylist a b c\n");
    std::string out = Exec(eng,"LRANGE mylist 0 -1\n");
    EXPECT_EQ(out, "1) \"a\"\n2) \"b\"\n3) \"c\"\n");
}

TEST(ListTest, LPopSingle) {
    engine::Engine eng;
    Exec(eng,"RPUSH mylist x y z\n");
    std::string out = Exec(eng,"LPOP mylist\n");
    EXPECT_EQ(out, "\"x\"\n");
}

TEST(ListTest, RPopSingle) {
    engine::Engine eng;
    Exec(eng,"RPUSH mylist x y z\n");
    std::string out = Exec(eng,"RPOP mylist\n");
    EXPECT_EQ(out, "\"z\"\n");
}

TEST(ListTest, PopMissingKey) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng,"LPOP nokey\n"), "(nil)\n");
    EXPECT_EQ(Exec(eng,"RPOP nokey\n"), "(nil)\n");
}

TEST(ListTest, LPopWithCount) {
    engine::Engine eng;
    Exec(eng,"RPUSH mylist a b c d\n");
    std::string out = Exec(eng,"LPOP mylist 2\n");
    EXPECT_EQ(out, "1) \"a\"\n2) \"b\"\n");
}

TEST(ListTest, LLenAndWrongType) {
    engine::Engine eng;
    Exec(eng,"SET strkey hello\n");
    EXPECT_EQ(Exec(eng,"LLEN strkey\n"), "(error) WRONGTYPE Operation against a key holding the wrong kind of value\n");
    EXPECT_EQ(Exec(eng,"LLEN missing\n"), "(int) 0\n");
    Exec(eng,"RPUSH mylist a b c\n");
    EXPECT_EQ(Exec(eng,"LLEN mylist\n"), "(int) 3\n");
}

TEST(ListTest, LRangeNegativeIndices) {
    engine::Engine eng;
    Exec(eng,"RPUSH mylist a b c\n");
    EXPECT_EQ(Exec(eng,"LRANGE mylist 0 -1\n"), "1) \"a\"\n2) \"b\"\n3) \"c\"\n");
    EXPECT_EQ(Exec(eng,"LRANGE mylist -2 -1\n"), "1) \"b\"\n2) \"c\"\n");
}

TEST(ListTest, LIndexBounds) {
    engine::Engine eng;
    Exec(eng,"RPUSH mylist x y z\n");
    EXPECT_EQ(Exec(eng,"LINDEX mylist 0\n"), "\"x\"\n");
    EXPECT_EQ(Exec(eng,"LINDEX mylist -1\n"), "\"z\"\n");
    EXPECT_EQ(Exec(eng,"LINDEX mylist 99\n"), "(nil)\n");
}

TEST(ListTest, LSetUpdate) {
    engine::Engine eng;
    Exec(eng,"RPUSH mylist a b c\n");
    EXPECT_EQ(Exec(eng,"LSET mylist 0 newval\n"), "OK\n");
    EXPECT_EQ(Exec(eng,"LINDEX mylist 0\n"), "\"newval\"\n");
    EXPECT_EQ(Exec(eng,"LSET nokey 0 v\n"), "(error) ERR no such key\n");
    EXPECT_EQ(Exec(eng,"LSET mylist 99 v\n"), "(error) ERR index out of range\n");
}

TEST(ListTest, LInsertBeforeAfter) {
    engine::Engine eng;
    Exec(eng,"RPUSH mylist a b c\n");
    EXPECT_EQ(Exec(eng,"LINSERT mylist BEFORE b X\n"), "(int) 4\n");
    EXPECT_EQ(Exec(eng,"LRANGE mylist 0 -1\n"), "1) \"a\"\n2) \"X\"\n3) \"b\"\n4) \"c\"\n");
    EXPECT_EQ(Exec(eng,"LINSERT mylist AFTER b Y\n"), "(int) 5\n");
    EXPECT_EQ(Exec(eng,"LRANGE mylist 0 -1\n"), "1) \"a\"\n2) \"X\"\n3) \"b\"\n4) \"Y\"\n5) \"c\"\n");
    EXPECT_EQ(Exec(eng,"LINSERT mylist BEFORE nopivot Z\n"), "(int) -1\n");
    EXPECT_EQ(Exec(eng,"LINSERT nokey BEFORE a Z\n"), "(int) 0\n");
}
