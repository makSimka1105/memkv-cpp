target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/eval.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/func.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/funccall.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/helpers.hpp
)
