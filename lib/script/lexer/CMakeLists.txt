target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/cursor.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/double_rule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ident_rule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/int_rule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/kw_rule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/op_rule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/punct_rule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rule_set.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/string_rule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scanner.hpp
)
