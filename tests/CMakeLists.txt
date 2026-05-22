include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

function(add_valkey_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE valkey_core GTest::gtest_main)
    gtest_discover_tests(${name} DISCOVERY_MODE POST_BUILD)
endfunction()

add_valkey_test(test_string)
add_valkey_test(test_transaction)
add_valkey_test(test_set)
add_valkey_test(test_list)
add_valkey_test(test_generic)
