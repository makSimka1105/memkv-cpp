target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/commands.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/token_extract.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/registry_types.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/registry.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser.hpp
)

add_subdirectory(lexer)
