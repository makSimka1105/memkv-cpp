target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rules/double.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rules/ident.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rules/int.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rules/kw.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rules/op.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rules/punct.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rules/string.hpp
)
