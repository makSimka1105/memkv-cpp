target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/lpush.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rpush.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lpop.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rpop.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/llen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lrange.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lindex.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lset.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/linsert.hpp
)
