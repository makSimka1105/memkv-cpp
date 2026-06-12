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

TEST(Generic, TypeOfStringListSet) {
    engine::Engine eng;
    Exec(eng, "SET mystr hello\nLPUSH mylist a\nSADD myset x\n");
    EXPECT_EQ(Exec(eng, "TYPE mystr\n"), "string\n");
    EXPECT_EQ(Exec(eng, "TYPE mylist\n"), "list\n");
    EXPECT_EQ(Exec(eng, "TYPE myset\n"), "set\n");
}

TEST(Generic, TypeMissingKey) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng, "TYPE nosuchkey\n"), "none\n");
}

TEST(Generic, KeysSingleKey) {
    engine::Engine eng;
    Exec(eng, "SET alpha beta\n");
    EXPECT_EQ(Exec(eng, "KEYS *\n"), "1) \"alpha\"\n");
}

TEST(Generic, KeysEmpty) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng, "KEYS *\n"), "(empty array)\n");
}

TEST(Generic, ExistsHitAndMiss) {
    engine::Engine eng;
    Exec(eng, "SET present 1\n");
    EXPECT_EQ(Exec(eng, "EXISTS present\n"), "(integer) 1\n");
    EXPECT_EQ(Exec(eng, "EXISTS absent\n"), "(integer) 0\n");
    EXPECT_EQ(Exec(eng, "EXISTS present present\n"), "(integer) 2\n");
}

TEST(Generic, DelExistingAndMissing) {
    engine::Engine eng;
    Exec(eng, "SET k1 v1\nSET k2 v2\n");
    EXPECT_EQ(Exec(eng, "DEL k1\n"), "(integer) 1\n");
    EXPECT_EQ(Exec(eng, "GET k1\n"), "(nil)\n");
    EXPECT_EQ(Exec(eng, "DEL ghost\n"), "(integer) 0\n");
    EXPECT_EQ(Exec(eng, "DEL k2 ghost\n"), "(integer) 1\n");
}

TEST(Generic, FlushDb) {
    engine::Engine eng;
    Exec(eng, "SET a 1\nSET b 2\n");
    EXPECT_EQ(Exec(eng, "FLUSHDB\n"), "OK\n");
    EXPECT_EQ(Exec(eng, "DBSIZE\n"), "(integer) 0\n");
}

TEST(Generic, DbSize) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng, "DBSIZE\n"), "(integer) 0\n");
    Exec(eng, "SET x 1\nSET y 2\n");
    EXPECT_EQ(Exec(eng, "DBSIZE\n"), "(integer) 2\n");
    Exec(eng, "DEL x\n");
    EXPECT_EQ(Exec(eng, "DBSIZE\n"), "(integer) 1\n");
}

TEST(Generic, MemoryUsageMissingKey) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng, "MEMORY USAGE ghost\n"), "(integer) 0\n");
}

TEST(Generic, MemoryUsageExistingKey) {
    engine::Engine eng;
    Exec(eng, "SET mykey somevalue\n");
    std::string result = Exec(eng, "MEMORY USAGE mykey\n");
    EXPECT_NE(result, "(integer) 0\n");
    EXPECT_NE(result, "(nil)\n");
}

TEST(Generic, ConfigGetMaxmemory) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng, "CONFIG GET maxmemory\n"), "1) \"maxmemory\"\n2) \"0\"\n");
}

TEST(Generic, ConfigSetAndGet) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng, "CONFIG SET maxmemory 1048576\n"), "OK\n");
    EXPECT_EQ(Exec(eng, "CONFIG GET maxmemory\n"), "1) \"maxmemory\"\n2) \"1048576\"\n");
}

TEST(Generic, ConfigGetUnknown) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng, "CONFIG GET nosuchparam\n"), "1) \"nosuchparam\"\n2) (nil)\n");
}

TEST(Generic, ConfigSetInvalidValue) {
    engine::Engine eng;
    std::string result = Exec(eng, "CONFIG SET maxmemory notanumber\n");
    EXPECT_EQ(result.substr(0, 7), "(error)");
}
