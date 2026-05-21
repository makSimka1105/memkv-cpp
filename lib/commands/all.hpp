#pragma once

#include "commands/del.hpp"
#include "commands/get.hpp"
#include "commands/set.hpp"
#include "meta/type_list.hpp"

namespace valkey::commands {

using AllCommandsTraits = meta::TraitsList<
    get::Trait,
    set::Trait,
    del::Trait
>;

}  // namespace valkey::commands
