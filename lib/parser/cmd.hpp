#pragma once

#include "commands/all.hpp"
#include "meta/command_tree.hpp"
#include "meta/type_list.hpp"

namespace valkey::parser {

using Cmd = VariantFromList<FlattenLeaves<commands::RootGroup>>;

}  // namespace valkey::parser
