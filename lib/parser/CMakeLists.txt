target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/errors.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/token_convert.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cursor.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cmd.hpp
)
