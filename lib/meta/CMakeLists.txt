target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/fixed_string.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/overload.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/type_list.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/try.hpp
)
