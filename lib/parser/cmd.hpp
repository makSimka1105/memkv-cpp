#pragma once

#include "commands/all.hpp"
#include "meta/type_list.hpp"

namespace valkey::parser {

using Cmd = meta::VariantFromList<commands::AllCommandsTraits>;

}  // namespace valkey::parser
