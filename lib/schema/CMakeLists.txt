target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/contract.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scan.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/apply.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/schema.hpp
)

add_subdirectory(strategies)
