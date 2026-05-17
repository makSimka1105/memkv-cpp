target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/types.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IHandler.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/chain_builder.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/quoted_string.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/whitespace.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/word_handler.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/int_handler.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/float_handler.hpp
)
