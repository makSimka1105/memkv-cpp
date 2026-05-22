target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/glob.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/storage.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/value.hpp
)
