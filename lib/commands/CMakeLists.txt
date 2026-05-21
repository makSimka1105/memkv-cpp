target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/all.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/get.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/set.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/del.hpp
)
