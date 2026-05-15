#!/usr/bin/env bash

set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

CLANG="clang++-21"
command -v "$CLANG" >/dev/null 2>&1 || CLANG="$(command -v clang++ || echo '')"

run() {
    local id="$1" cxx="$2" flags="$3"
    local dir="build-$id"
    echo "==> [$id] $cxx ${flags:-(no flags)}"
    cmake -S . -B "$dir" -DCMAKE_CXX_COMPILER="$cxx" -DCMAKE_CXX_FLAGS="$flags"
    cmake --build "$dir" -j
    ctest --test-dir "$dir" --output-on-failure
}

run gcc      g++   ""
run gcc_san  g++   "-fsanitize=address,undefined"
if [[ -n "$CLANG" ]]; then
    run clang     "$CLANG" ""
    run clang_san "$CLANG" "-fsanitize=address,undefined"
else
    echo "==> clang not installed; skipping clang/clang_san"
fi

echo "==> All checks passed"
