target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/config_get.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/config_set.hpp
)
