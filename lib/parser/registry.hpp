#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include <variant>

#include "commands/all.hpp"
#include "meta/type_list.hpp"
#include "meta/variant_index.hpp"
#include "parser/commands.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::parser {

namespace detail {

template <class... Traits>
constexpr auto BuildCommandRegistry(meta::TraitsList<Traits...>) {
    static_assert(sizeof...(Traits) == std::variant_size_v<Cmd>,
                  "AllCommands must list one Trait per Cmd variant alternative");
    std::array<CommandEntry, std::variant_size_v<Cmd>> table{};
    ((table[meta::kVariantIndex<typename Traits::Node, Cmd>] = Traits::kEntry), ...);
    return table;
}

}  // namespace detail

constexpr auto kCommands = detail::BuildCommandRegistry(commands::AllCommandsTraits{});

constexpr const CommandEntry* Lookup(std::string_view name) {
    for (const auto& entry : kCommands) {
        if (CaseEqual(entry.name, name)) {
            return &entry;
        }
    }
    return nullptr;
}

constexpr bool ArityFits(ArityRange r, size_t argc) {
    return argc >= r.min && argc <= r.max;
}

}  // namespace valkey::parser
