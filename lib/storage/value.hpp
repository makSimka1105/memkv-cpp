#pragma once

#include "meta/type_list.hpp"
#include "types/all.hpp"

namespace valkey::storage {

using ValueVar = VariantFromList<types::AllStoredTypes>;

}  // namespace valkey::storage
