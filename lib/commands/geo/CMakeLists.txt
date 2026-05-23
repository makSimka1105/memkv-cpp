target_sources(valkey_core
    INTERFACE
    FILE_SET HEADERS
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/geo_nodes.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geoadd.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geopos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geodist.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geohash_cmd.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geosearch.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/geosearchstore.hpp
)
