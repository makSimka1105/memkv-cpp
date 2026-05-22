target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/del.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exists.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/type.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/keys.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/flushdb.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dbsize.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/memory_usage.hpp
)
