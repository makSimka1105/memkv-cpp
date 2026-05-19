target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/overload.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/type_list.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/variant_index.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dispatcher.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/command_trait.hpp
)
