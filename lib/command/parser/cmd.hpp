#pragma once

#include "command/commands/all.hpp"
#include "command/commands/tree.hpp"
#include "meta/type_list.hpp"

namespace valkey::parser {

using Cmd = VariantFromList<commands::FlattenLeaves<commands::RootGroup>>;

}  // namespace valkey::parser
