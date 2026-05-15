cmake_minimum_required(VERSION 3.23)
project(valkey LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(VALKEY_SANITIZE "Build with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

if(VALKEY_SANITIZE)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

enable_testing()

add_subdirectory(lib)
add_subdirectory(bin)
add_subdirectory(tests)
