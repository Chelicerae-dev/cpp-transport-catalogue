cmake_minimum_required(VERSION 3.10)

project(transport_catalogue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH ../../protobuf/debug/)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_catalogue.proto)
protobuf_generate_cpp(PROTO_MAP_SRCS PROTO_MAP_HDRS map_renderer.proto)
protobuf_generate_cpp(PROTO_SVG_SRCS PROTO_SVG_HDRS svg.proto)
protobuf_generate_cpp(PROTO_GRAPH_SRCS PROTO_GRAPH_HDRS graph.proto)
protobuf_generate_cpp(PROTO_ROUTER_SRCS PROTO_ROUTER_HDRS transport_router.proto)

set(PROJECT_FILES domain.h domain.cpp
                geo.h geo.cpp
                graph.h
                json.h json.cpp
                json_builder.h json_builder.cpp
                json_reader.h json_reader.cpp
                map_renderer.h map_renderer.cpp
                ranges.h
                request_handler.h request_handler.cpp
                router.h
                ranges.h
                serialization.h serialization.cpp
                svg.h svg.cpp
                transport_catalogue.h transport_catalogue.cpp
                transport_router.h transport_router.cpp )

add_executable(transport_catalogue main.cpp ${PROJECT_FILES}
            transport_catalogue.proto ${PROTO_SRCS} ${PROTO_HDRS}
            map_renderer.proto ${PROTO_MAP_SRCS} ${PROTO_MAP_HDRS}
            svg.proto ${PROTO_SVG_SRCS} ${PROTO_SVG_HDRS}
            graph.proto ${PROTO_GRAPH_SRCS} ${PROTO_GRAPH_HDRS}
            transport_router.proto ${PROTO_ROUTER_SRCS} ${PROTO_ROUTER_HDRS}
        )

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)

install(TARGETS transport_catalogue
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
