#pragma once

#include "config/config_get.hpp"
#include "config/config_set.hpp"
#include "general/dbsize.hpp"
#include "general/exists.hpp"
#include "general/flushdb.hpp"
#include "general/keys.hpp"
#include "general/memory_usage.hpp"
#include "general/del.hpp"
#include "general/type.hpp"
#include "list/lindex.hpp"
#include "list/linsert.hpp"
#include "list/llen.hpp"
#include "list/lpop.hpp"
#include "list/lpush.hpp"
#include "list/lrange.hpp"
#include "list/lset.hpp"
#include "list/rpop.hpp"
#include "list/rpush.hpp"
#include "string/get.hpp"
#include "string/set.hpp"
#include "set/sadd.hpp"
#include "set/scard.hpp"
#include "set/sdiff.hpp"
#include "set/sinter.hpp"
#include "set/sismember.hpp"
#include "set/smembers.hpp"
#include "set/smove.hpp"
#include "set/srem.hpp"
#include "set/sunion.hpp"
#include "string/append.hpp"
#include "string/expire.hpp"
#include "string/strlen.hpp"
#include "string/ttl.hpp"
#include "transaction/discard.hpp"
#include "transaction/exec.hpp"
#include "transaction/multi.hpp"
#include "meta/command_tree.hpp"

namespace valkey::commands {

using RootGroup = CommandGroup<"",
    LeafCommand<GetTrait>,
    LeafCommand<SetTrait>,
    LeafCommand<StrLenTrait>,
    LeafCommand<AppendTrait>,
    LeafCommand<ExpireTrait>,
    LeafCommand<TtlTrait>,
    LeafCommand<DelTrait>,
    LeafCommand<ExistsTrait>,
    LeafCommand<TypeTrait>,
    LeafCommand<KeysTrait>,
    LeafCommand<FlushDbTrait>,
    LeafCommand<DbSizeTrait>,
    LeafCommand<LPushTrait>,
    LeafCommand<RPushTrait>,
    LeafCommand<LPopTrait>,
    LeafCommand<RPopTrait>,
    LeafCommand<LLenTrait>,
    LeafCommand<LRangeTrait>,
    LeafCommand<LIndexTrait>,
    LeafCommand<LSetTrait>,
    LeafCommand<LInsertTrait>,
    LeafCommand<SAddTrait>,
    LeafCommand<SRemTrait>,
    LeafCommand<SCardTrait>,
    LeafCommand<SIsMemberTrait>,
    LeafCommand<SMembersTrait>,
    LeafCommand<SUnionTrait>,
    LeafCommand<SInterTrait>,
    LeafCommand<SDiffTrait>,
    LeafCommand<SMoveTrait>,
    CommandGroup<"CONFIG",
        LeafCommand<ConfigGetTrait>,
        LeafCommand<ConfigSetTrait>
    >,
    CommandGroup<"MEMORY",
        LeafCommand<MemoryUsageTrait>
    >,
    LeafCommand<MultiTrait>,
    LeafCommand<ExecTrait>,
    LeafCommand<DiscardTrait>
>;

}  // namespace valkey::commands
