target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/char_class.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/engine.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/quoted.hpp
)
