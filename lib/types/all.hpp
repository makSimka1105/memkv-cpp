#pragma once

#include "meta/type_list.hpp"
#include "types/geo.hpp"
#include "types/list.hpp"
#include "types/set.hpp"
#include "types/string.hpp"
#include "types/zset.hpp"

namespace valkey::types {

using AllStoredTypes = TraitsList<StringTrait, ListTrait, SetTrait, ZSetTrait, GeoTrait>;

using String = StringTrait::Type;
using List   = ListTrait::Type;
using Set    = SetTrait::Type;
using ZSet   = ZSetTrait::Type;
using Geo    = GeoTrait::Type;

}  // namespace valkey::types
