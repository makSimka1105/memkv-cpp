target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/skiplist.hpp
)
