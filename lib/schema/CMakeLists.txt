target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/contract.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scan.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/apply.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/schema.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/command_trait.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/command_tree.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/alt_group.hpp
)

add_subdirectory(strategies)
