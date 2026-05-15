#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

#include "commands/append.hpp"
#include "commands/config.hpp"
#include "commands/dbsize.hpp"
#include "commands/del.hpp"
#include "commands/exists.hpp"
#include "commands/expire.hpp"
#include "commands/flushdb.hpp"
#include "commands/get.hpp"
#include "commands/keys.hpp"
#include "commands/lindex.hpp"
#include "commands/linsert.hpp"
#include "commands/llen.hpp"
#include "commands/lpop.hpp"
#include "commands/lpush.hpp"
#include "commands/lrange.hpp"
#include "commands/lset.hpp"
#include "commands/memory_usage.hpp"
#include "commands/rpop.hpp"
#include "commands/rpush.hpp"
#include "commands/sadd.hpp"
#include "commands/scard.hpp"
#include "commands/sdiff.hpp"
#include "commands/set.hpp"
#include "commands/sinter.hpp"
#include "commands/sismember.hpp"
#include "commands/smembers.hpp"
#include "commands/smove.hpp"
#include "commands/srem.hpp"
#include "commands/strlen.hpp"
#include "commands/sunion.hpp"
#include "commands/ttl.hpp"
#include "commands/type.hpp"
#include "parser/commands.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::parser {

namespace detail {

template <typename T, typename V, size_t... Is>
constexpr size_t VariantIndexImpl(std::index_sequence<Is...>) {
    size_t found = sizeof...(Is);
    ((std::is_same_v<std::variant_alternative_t<Is, V>, T> ? (found = Is, 0) : 0), ...);
    return found;
}

}  // namespace detail

template <typename T>
constexpr size_t kVariantIndex =
    detail::VariantIndexImpl<T, Cmd>(std::make_index_sequence<std::variant_size_v<Cmd>>{});

constexpr auto kCommands = []() {
    std::array<CommandEntry, std::variant_size_v<Cmd>> table{};
    table[kVariantIndex<cmd_nodes::Set>] = commands::set::kEntry;
    table[kVariantIndex<cmd_nodes::Get>] = commands::get::kEntry;
    table[kVariantIndex<cmd_nodes::Del>] = commands::del::kEntry;
    table[kVariantIndex<cmd_nodes::Exists>] = commands::exists::kEntry;
    table[kVariantIndex<cmd_nodes::Strlen>] = commands::strlen::kEntry;
    table[kVariantIndex<cmd_nodes::Append>] = commands::append::kEntry;
    table[kVariantIndex<cmd_nodes::Expire>] = commands::expire::kEntry;
    table[kVariantIndex<cmd_nodes::Ttl>] = commands::ttl::kEntry;
    table[kVariantIndex<cmd_nodes::LPush>] = commands::lpush::kEntry;
    table[kVariantIndex<cmd_nodes::RPush>] = commands::rpush::kEntry;
    table[kVariantIndex<cmd_nodes::LPop>] = commands::lpop::kEntry;
    table[kVariantIndex<cmd_nodes::RPop>] = commands::rpop::kEntry;
    table[kVariantIndex<cmd_nodes::LLen>] = commands::llen::kEntry;
    table[kVariantIndex<cmd_nodes::LRange>] = commands::lrange::kEntry;
    table[kVariantIndex<cmd_nodes::LIndex>] = commands::lindex::kEntry;
    table[kVariantIndex<cmd_nodes::LSet>] = commands::lset::kEntry;
    table[kVariantIndex<cmd_nodes::LInsert>] = commands::linsert::kEntry;
    table[kVariantIndex<cmd_nodes::SAdd>] = commands::sadd::kEntry;
    table[kVariantIndex<cmd_nodes::SRem>] = commands::srem::kEntry;
    table[kVariantIndex<cmd_nodes::SIsMember>] = commands::sismember::kEntry;
    table[kVariantIndex<cmd_nodes::SMembers>] = commands::smembers::kEntry;
    table[kVariantIndex<cmd_nodes::SCard>] = commands::scard::kEntry;
    table[kVariantIndex<cmd_nodes::SUnion>] = commands::sunion::kEntry;
    table[kVariantIndex<cmd_nodes::SInter>] = commands::sinter::kEntry;
    table[kVariantIndex<cmd_nodes::SDiff>] = commands::sdiff::kEntry;
    table[kVariantIndex<cmd_nodes::SMove>] = commands::smove::kEntry;
    table[kVariantIndex<cmd_nodes::Type>] = commands::type::kEntry;
    table[kVariantIndex<cmd_nodes::Keys>] = commands::keys::kEntry;
    table[kVariantIndex<cmd_nodes::FlushDb>] = commands::flushdb::kEntry;
    table[kVariantIndex<cmd_nodes::Config>] = commands::config::kEntry;
    table[kVariantIndex<cmd_nodes::DBSize>] = commands::dbsize::kEntry;
    table[kVariantIndex<cmd_nodes::Memory>] = commands::memory_usage::kEntry;
    return table;
}();

constexpr const CommandEntry* Lookup(std::string_view name) {
    for (const auto& entry : kCommands) {
        if (CaseEqual(entry.name, name)) {
            return &entry;
        }
    }
    return nullptr;
}

constexpr bool ArityFits(ArityRange r, size_t argc) {
    return argc >= r.min && argc <= r.max;
}

}  // namespace valkey::parser
