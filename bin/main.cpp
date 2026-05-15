#include <iostream>
#include <string_view>

#include "engine/config.hpp"
#include "engine/engine.hpp"
#include "io/stdin_frontend.hpp"

int main(int argc, char* argv[]) {
    valkey::engine::Config cfg;

    for (int i = 1; i < argc; ++i) {
        std::string_view arg{argv[i]};
        if (arg == "--maxmemory" && i + 1 < argc) {
            auto bytes = valkey::engine::ParseMemorySize(argv[++i]);
            if (!bytes) {
                std::cerr << "invalid --maxmemory value: " << argv[i] << '\n';
                return 1;
            }
            cfg.maxmemory = *bytes;
        }
    }

    valkey::engine::Engine eng{cfg};
    valkey::io::RunStdinRepl(eng, std::cin, std::cout);
    return 0;
}
