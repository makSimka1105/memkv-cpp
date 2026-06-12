target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/types.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/util.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rules/quoted_string.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rules/int.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rules/float.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rules/word.hpp
)
