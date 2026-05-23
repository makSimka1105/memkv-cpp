#pragma once

#include "meta/type_list.hpp"
#include "types/list.hpp"
#include "types/set.hpp"
#include "types/string.hpp"
#include "types/zset.hpp"

namespace valkey::types {

using AllStoredTypes = TraitsList<StringTrait, ListTrait, SetTrait, ZSetTrait>;

using String = StringTrait::Type;
using List   = ListTrait::Type;
using Set    = SetTrait::Type;
using ZSet   = ZSetTrait::Type;

}  // namespace valkey::types
