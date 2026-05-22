target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/get.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/set.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/strlen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/append.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/expire.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ttl.hpp
)
