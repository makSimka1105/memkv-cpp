cmake_minimum_required(VERSION 3.23)
project(valkey LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(VALKEY_SANITIZE "Build with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
option(VALKEY_TESTS "Build unit tests (downloads googletest)" ON)

add_compile_options(-Wall -Wextra -Wpedantic)

if(VALKEY_SANITIZE)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

add_subdirectory(lib)
add_subdirectory(bin)

if(VALKEY_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
