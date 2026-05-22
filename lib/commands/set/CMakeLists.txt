target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/sadd.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/srem.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scard.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sismember.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/smembers.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sunion.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sinter.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sdiff.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/smove.hpp
)
