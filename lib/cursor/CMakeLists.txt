target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/base_cursor.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/char_cursor.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/token_cursor.hpp
)
