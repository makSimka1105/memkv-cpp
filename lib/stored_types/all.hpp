#pragma once

#include "meta/type_list.hpp"
#include "stored_types/string.hpp"
#include "stored_types/list.hpp"
#include "stored_types/set.hpp"
#include "stored_types/zset.hpp"

namespace valkey::stored_types {

using AllStoredTypes = TraitsList<string::Trait, list::Trait, set::Trait, zset::Trait>;

}  // namespace valkey::stored_types
