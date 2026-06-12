target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/zadd.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zrem.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zscore.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zcard.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zrank.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zrevrank.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zrange.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zrevrange.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zrangebyscore.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zincrby.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zpopmin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zpopmax.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/zcount.hpp
)
