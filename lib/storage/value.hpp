#pragma once

#include "meta/type_list.hpp"
#include "stored_types/all.hpp"

namespace valkey::storage {

using ValueVar = meta::VariantFromList<stored_types::AllStoredTypes>;

}  // namespace valkey::storage
