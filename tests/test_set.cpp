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

TEST(SetTest, SaddDeduplication) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng, "SADD s a b c"), "(integer) 3\n");
    EXPECT_EQ(Exec(eng, "SADD s a"), "(integer) 0\n");
    EXPECT_EQ(Exec(eng, "SADD s d"), "(integer) 1\n");
    EXPECT_EQ(Exec(eng, "SCARD s"), "(integer) 4\n");
}

TEST(SetTest, SremExistingAndMissing) {
    engine::Engine eng;
    Exec(eng, "SADD s x y");
    EXPECT_EQ(Exec(eng, "SREM s x"), "(integer) 1\n");
    EXPECT_EQ(Exec(eng, "SREM s x"), "(integer) 0\n");
    EXPECT_EQ(Exec(eng, "SREM nokey z"), "(integer) 0\n");
}

TEST(SetTest, ScardEmpty) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng, "SCARD nokey"), "(integer) 0\n");
}

TEST(SetTest, SismemberHitAndMiss) {
    engine::Engine eng;
    Exec(eng, "SADD s hello");
    EXPECT_EQ(Exec(eng, "SISMEMBER s hello"), "(integer) 1\n");
    EXPECT_EQ(Exec(eng, "SISMEMBER s world"), "(integer) 0\n");
    EXPECT_EQ(Exec(eng, "SISMEMBER nokey hello"), "(integer) 0\n");
}

TEST(SetTest, SmembersEmpty) {
    engine::Engine eng;
    EXPECT_EQ(Exec(eng, "SMEMBERS nokey"), "(empty array)\n");
}

TEST(SetTest, SmembersSingle) {
    engine::Engine eng;
    Exec(eng, "SADD s only");
    EXPECT_EQ(Exec(eng, "SMEMBERS s"), "1) \"only\"\n");
}

TEST(SetTest, SWrongType) {
    engine::Engine eng;
    Exec(eng, "SET k value");
    EXPECT_EQ(Exec(eng, "SADD k member"), "(error) WRONGTYPE Operation against a key holding the wrong kind of value\n");
    EXPECT_EQ(Exec(eng, "SREM k member"), "(error) WRONGTYPE Operation against a key holding the wrong kind of value\n");
    EXPECT_EQ(Exec(eng, "SCARD k"), "(error) WRONGTYPE Operation against a key holding the wrong kind of value\n");
}

TEST(SetTest, SmoveMemberAcrossSets) {
    engine::Engine eng;
    Exec(eng, "SADD src a b");
    Exec(eng, "SADD dst c");
    EXPECT_EQ(Exec(eng, "SMOVE src dst a"), "(integer) 1\n");
    EXPECT_EQ(Exec(eng, "SISMEMBER src a"), "(integer) 0\n");
    EXPECT_EQ(Exec(eng, "SISMEMBER dst a"), "(integer) 1\n");
}

TEST(SetTest, SmoveMissingMember) {
    engine::Engine eng;
    Exec(eng, "SADD src x");
    EXPECT_EQ(Exec(eng, "SMOVE src dst z"), "(integer) 0\n");
    EXPECT_EQ(Exec(eng, "SMOVE nokey dst x"), "(integer) 0\n");
    EXPECT_EQ(Exec(eng, "SCARD dst"), "(integer) 0\n");
}

TEST(SetTest, SdiffBasic) {
    engine::Engine eng;
    Exec(eng, "SADD a only");
    Exec(eng, "SADD b other");
    EXPECT_EQ(Exec(eng, "SDIFF a b"), "1) \"only\"\n");
    EXPECT_EQ(Exec(eng, "SDIFF b a"), "1) \"other\"\n");
}

TEST(SetTest, SinterBasic) {
    engine::Engine eng;
    Exec(eng, "SADD a common");
    Exec(eng, "SADD b common");
    EXPECT_EQ(Exec(eng, "SINTER a b"), "1) \"common\"\n");
}

TEST(SetTest, SunionBasic) {
    engine::Engine eng;
    Exec(eng, "SADD a alpha");
    Exec(eng, "SADD b beta");
    EXPECT_EQ(Exec(eng, "SCARD a"), "(integer) 1\n");
    EXPECT_EQ(Exec(eng, "SCARD b"), "(integer) 1\n");
    EXPECT_EQ(Exec(eng, "SISMEMBER a alpha"), "(integer) 1\n");
    EXPECT_EQ(Exec(eng, "SISMEMBER b beta"), "(integer) 1\n");
    Exec(eng, "SADD union_result alpha beta");
    EXPECT_EQ(Exec(eng, "SCARD union_result"), "(integer) 2\n");
    EXPECT_EQ(Exec(eng, "SDIFF union_result a b"), "(empty array)\n");
}
