target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/multi.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exec.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/discard.hpp
)
