target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/ast.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/box.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/error.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/token.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/value.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/runtime.hpp
)

add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(evaluator)
