add_library(valkey_core INTERFACE)

target_include_directories(valkey_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(meta)
add_subdirectory(structures)
add_subdirectory(stored_types)
add_subdirectory(storage)
add_subdirectory(engine)
add_subdirectory(parser)
add_subdirectory(commands)
add_subdirectory(io)
