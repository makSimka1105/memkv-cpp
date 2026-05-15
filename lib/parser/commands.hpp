#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace valkey::cmd_nodes {

struct Set {
    std::string key;
    std::string value;
};

struct Get {
    std::string key;
};

struct Del {
    std::vector<std::string> keys;
};

struct Exists {
    std::vector<std::string> keys;
};

struct Strlen {
    std::string key;
};

struct Append {
    std::string key;
    std::string value;
};

struct Expire {
    std::string key;
    int64_t seconds;
};

struct Ttl {
    std::string key;
};

struct LPush {
    std::string key;
    std::vector<std::string> values;
};

struct RPush {
    std::string key;
    std::vector<std::string> values;
};

struct LPop {
    std::string key;
    std::optional<int64_t> count;
};

struct RPop {
    std::string key;
    std::optional<int64_t> count;
};

struct LLen {
    std::string key;
};

struct LRange {
    std::string key;
    int64_t start;
    int64_t stop;
};

struct LIndex {
    std::string key;
    int64_t index;
};

struct LSet {
    std::string key;
    int64_t index;
    std::string value;
};

struct LInsert {
    std::string key;
    bool before;
    std::string pivot;
    std::string value;
};

struct SAdd {
    std::string key;
    std::vector<std::string> members;
};

struct SRem {
    std::string key;
    std::vector<std::string> members;
};

struct SIsMember {
    std::string key;
    std::string member;
};

struct SMembers {
    std::string key;
};

struct SCard {
    std::string key;
};

struct SUnion {
    std::vector<std::string> keys;
};

struct SInter {
    std::vector<std::string> keys;
};

struct SDiff {
    std::vector<std::string> keys;
};

struct SMove {
    std::string src;
    std::string dst;
    std::string member;
};

struct Type {
    std::string key;
};

struct Keys {
    std::string pattern;
};

struct FlushDb {};

struct Config {
    bool is_set;
    std::string param;
    std::string value;
};

struct DBSize {};

struct Memory {
    std::string subcommand;
    std::string key;
};

}  // namespace valkey::cmd_nodes

namespace valkey::parser {

using Cmd =
    std::variant<cmd_nodes::Set, cmd_nodes::Get, cmd_nodes::Del, cmd_nodes::Exists,
                 cmd_nodes::Strlen, cmd_nodes::Append, cmd_nodes::Expire, cmd_nodes::Ttl,
                 cmd_nodes::LPush, cmd_nodes::RPush, cmd_nodes::LPop, cmd_nodes::RPop,
                 cmd_nodes::LLen, cmd_nodes::LRange, cmd_nodes::LIndex, cmd_nodes::LSet,
                 cmd_nodes::LInsert, cmd_nodes::SAdd, cmd_nodes::SRem, cmd_nodes::SIsMember,
                 cmd_nodes::SMembers, cmd_nodes::SCard, cmd_nodes::SUnion, cmd_nodes::SInter,
                 cmd_nodes::SDiff, cmd_nodes::SMove, cmd_nodes::Type, cmd_nodes::Keys,
                 cmd_nodes::FlushDb, cmd_nodes::Config, cmd_nodes::DBSize, cmd_nodes::Memory>;

}  // namespace valkey::parser
