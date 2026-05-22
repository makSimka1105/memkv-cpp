target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/config.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/engine.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/memory_tracker.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/replies.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/interpreter.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/session.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/session_eval.hpp
)
