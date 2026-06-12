target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_SOURCE_DIR}/lib
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/diagnostic.hpp
)

add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(schema)
add_subdirectory(commands)
