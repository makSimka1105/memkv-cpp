#pragma once

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
#include "commands/zadd.hpp"
#include "commands/zcard.hpp"
#include "commands/zincrby.hpp"
#include "commands/zrange.hpp"
#include "commands/zrangebyscore.hpp"
#include "commands/zrank.hpp"
#include "commands/zrem.hpp"
#include "commands/zscore.hpp"
#include "meta/type_list.hpp"

namespace valkey::commands {

using AllCommandsTraits = meta::TraitsList<
    set::Trait, get::Trait, del::Trait, exists::Trait, strlen::Trait, append::Trait, expire::Trait,
    ttl::Trait, lpush::Trait, rpush::Trait, lpop::Trait, rpop::Trait, llen::Trait, lrange::Trait,
    lindex::Trait, lset::Trait, linsert::Trait, sadd::Trait, srem::Trait, sismember::Trait,
    smembers::Trait, scard::Trait, sunion::Trait, sinter::Trait, sdiff::Trait, smove::Trait,
    type::Trait, keys::Trait, flushdb::Trait, config::Trait, dbsize::Trait, memory_usage::Trait,
    zadd::Trait, zrem::Trait, zscore::Trait, zcard::Trait, zrank::Trait, zrange::Trait,
    zrangebyscore::Trait, zincrby::Trait>;

}  // namespace valkey::commands
