cmake_minimum_required(VERSION 3.0)
include (cmake/utils.cmake)
project(sylar)



set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAG = "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -g -std=c++11 -Wall -Werror -Wall  -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
add_definitions(-Wno-builtin-macro-redefined)
include_directories(.)
set(LIB_SRC
    sylar/log.cc
    sylar/util.cc
    sylar/config.cc
    sylar/thread.cc
    sylar/fiber.cc
    sylar/scheduler.cc
    sylar/iomanager.cc
    sylar/timer.cc
    sylar/hook.cc
    sylar/fd_manager.cc
    sylar/address.cc
    sylar/socket.cc
    sylar/bytearray.cc
    sylar/http/http.cc
    sylar/http/http11_parser.rl.cc
    sylar/http/httpclient_parser.rl.cc
    sylar/http/http_parser.cc
    sylar/tcp_server.cc
    sylar/stream.cc
    sylar/socket_stream.cc
    sylar/http/http_session.cc
    sylar/http/http_server.cc
    sylar/http/servlet.cc
    sylar/http/http_connection.cc
    sylar/uri.rl.cc
    )

ragelmaker(sylar/uri.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sylar)

add_library(sylar SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(sylar)
#add_library(sylar_static STATIC $(LIB_SRC))
#SET_TARGET_PROPERTIES(sylar_static PROPERTIES OUTPUT_NAME "sylar")

set(LIB_LIB
    pthread
    yaml-cpp 
    dl
)

# sylar_add_executable(test_config "tests/test_config.cc" sylar "${LIB_LIB}")

add_executable(test_thread tests/test_thread.cc)
target_link_libraries(test_thread sylar ${LIB_LIB})

add_executable(test_util tests/test_util.cc)
target_link_libraries(test_util sylar ${LIB_LIB})

add_executable(test_fiber tests/test_fiber.cc)
target_link_libraries(test_fiber sylar ${LIB_LIB})

add_executable(test_scheduler tests/test_scheduler.cc)
target_link_libraries(test_scheduler sylar ${LIB_LIB})

add_executable(test_iomanager tests/test_iomanager.cc)
target_link_libraries(test_iomanager sylar ${LIB_LIB})

add_executable(test_hook tests/test_hook.cc)
target_link_libraries(test_hook sylar ${LIB_LIB})

add_executable(test_address tests/test_address.cc)
target_link_libraries(test_address sylar ${LIB_LIB})

add_executable(test_socket tests/test_socket.cc)
target_link_libraries(test_socket sylar ${LIB_LIB})

add_executable(test_bytearray tests/test_bytearray.cc)
target_link_libraries(test_bytearray sylar ${LIB_LIB})

add_executable(test_http tests/test_http.cc)
target_link_libraries(test_http sylar ${LIB_LIB})

add_executable(test_httpparser tests/test_httpparser.cc)
target_link_libraries(test_httpparser sylar ${LIB_LIB})

add_executable(test_tcpserver tests/test_tcpserver.cc)
target_link_libraries(test_tcpserver sylar ${LIB_LIB})

add_executable(echo_server sylar/examples/echo_server.cc)
target_link_libraries(echo_server sylar ${LIB_LIB})

add_executable(testhttp_server sylar/examples/testhttp_server.cc)
target_link_libraries(testhttp_server sylar ${LIB_LIB})

add_executable(test_httpserver tests/test_httpserver.cc)
target_link_libraries(test_httpserver sylar ${LIB_LIB})

add_executable(test_httpconnection tests/test_httpconnection.cc)
target_link_libraries(test_httpconnection sylar ${LIB_LIB})

add_executable(test_httpclient.rl tests/test_httpclient.rl.cc)
target_link_libraries(test_httpclient.rl sylar ${LIB_LIB})

add_executable(test_uri tests/test_uri.cc)
target_link_libraries(test_uri sylar ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)