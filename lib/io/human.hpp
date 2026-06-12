#pragma once

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <variant>

#include "reply.hpp"
#include "meta/overload.hpp"

namespace valkey::io::human {

// forward declarations
void Write(std::ostream& out, const Reply& r, const std::string& indent = "");

namespace detail {

// width of "N) " prefix for a given 1-based index
std::string Pad(size_t one_based_idx) {
    std::string p;
    size_t n = one_based_idx;
    do { p += ' '; n /= 10; } while (n > 0);
    p += "  ";  // ") "
    return p;
}

void WriteScalar(std::ostream& out, const Reply& r) {
    std::visit(
        Overload{
            [&](const SimpleStr& s) { out << s.value << '\n'; },
            [&](const BulkStr& b) { out << '"' << b.value << '"' << '\n'; },
            [&](const Int& i) { out << "(integer) " << i.value << '\n'; },
            [&](const Bool& b) { out << (b.value ? "(true)" : "(false)") << '\n'; },
            [&](const Err& e) { out << "(error) " << e.message << '\n'; },
            [&](const Nil&) { out << "(nil)\n"; },
            [&](const std::unique_ptr<Array>&) {},
        },
        r);
}

// Write a reply assuming cursor is already positioned after the parent prefix.
// child_indent = indent to prepend to sibling elements (2nd, 3rd, ...) of nested arrays.
void WriteInline(std::ostream& out, const Reply& r, const std::string& child_indent);

void WriteInlineArray(std::ostream& out, const Array& a, const std::string& child_indent) {
    if (a.items.empty()) {
        out << "(empty array)\n";
        return;
    }
    out << "1) ";
    WriteInline(out, a.items[0], child_indent + Pad(1));
    for (size_t i = 1; i < a.items.size(); ++i) {
        out << child_indent << (i + 1) << ") ";
        WriteInline(out, a.items[i], child_indent + Pad(i + 1));
    }
}

void WriteInline(std::ostream& out, const Reply& r, const std::string& child_indent) {
    if (const auto* arr = std::get_if<std::unique_ptr<Array>>(&r)) {
        WriteInlineArray(out, **arr, child_indent);
    } else {
        WriteScalar(out, r);
    }
}

}  // namespace detail

void Write(std::ostream& out, const Reply& r, const std::string& indent) {
    if (const auto* arr = std::get_if<std::unique_ptr<Array>>(&r)) {
        if ((*arr)->items.empty()) {
            out << "(empty array)\n";
            return;
        }
        for (size_t i = 0; i < (*arr)->items.size(); ++i) {
            out << indent << (i + 1) << ") ";
            detail::WriteInline(out, (*arr)->items[i], indent + detail::Pad(i + 1));
        }
    } else {
        detail::WriteScalar(out, r);
    }
}

}  // namespace valkey::io::human
