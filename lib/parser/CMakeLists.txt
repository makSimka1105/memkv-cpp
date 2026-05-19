target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/token_extract.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cmd.hpp
)

add_subdirectory(lexer)
