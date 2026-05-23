target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/arg.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/variadic.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/flag.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/choice.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/literal.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/keyword.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/repeat.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/one_of.hpp
)
