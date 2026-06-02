#pragma once

#include "commands/all.hpp"
#include "commands/tree.hpp"
#include "meta/type_list.hpp"

namespace valkey::parser {

using Cmd = VariantFromList<commands::FlattenLeaves<commands::RootGroup>>;

}  // namespace valkey::parser
