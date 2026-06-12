target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/all.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/string.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/list.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/set.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zset.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geo.hpp
)
