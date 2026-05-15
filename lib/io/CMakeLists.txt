target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/reply.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/human.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/stdin_frontend.hpp
)
