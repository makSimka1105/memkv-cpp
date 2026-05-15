#pragma once

#include <cstddef>
#include <memory>
#include <ostream>
#include <variant>

#include "reply.hpp"
#include "meta/overload.hpp"

namespace valkey::io::human {

inline void Write(std::ostream& out, const Reply& r);

namespace detail {

void WriteItem(std::ostream& out, size_t one_based_idx, const Reply& r) {
    out << one_based_idx << ") ";
    Write(out, r);
}

}  // namespace detail

void Write(std::ostream& out, const Reply& r) {
    std::visit(
        meta::Overload{
            [&](const SimpleStr& s) { out << s.value << '\n'; },
            [&](const BulkStr& b) { out << '"' << b.value << '"' << '\n'; },
            [&](const Int& i) { out << "(int) " << i.value << '\n'; },
            [&](const Err& e) { out << "(error) " << e.message << '\n'; },
            [&](const Nil&) { out << "(nil)\n"; },
            [&](const std::unique_ptr<Array>& a) {
                if (a->items.empty()) {
                    out << "(empty array)\n";
                    return;
                }
                for (size_t idx = 0; idx < a->items.size(); ++idx) {
                    detail::WriteItem(out, idx + 1, a->items[idx]);
                }
            },
        },
        r);
}

}  // namespace valkey::io::human
