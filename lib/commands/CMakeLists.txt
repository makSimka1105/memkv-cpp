target_include_directories(valkey_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/all.hpp
)

add_subdirectory(config)
add_subdirectory(string)
add_subdirectory(general)
add_subdirectory(list)
add_subdirectory(set)
add_subdirectory(transaction)
add_subdirectory(zset)
add_subdirectory(geo)
