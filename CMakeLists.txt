cmake_minimum_required(VERSION 3.0)
include (cmake/utils.cmake)
project(sylar)



set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAG = "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno_unused-function")
add_definitions(-Wno-builtin-macro-redefined)
include_directories(.)
set(LIB_SRC
    sylar/log.cc
    sylar/util.cc
    sylar/config.cc
    sylar/thread.cc
    sylar/fiber.cc
    )

add_library(sylar SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(sylar)
#add_library(sylar_static STATIC $(LIB_SRC))
#SET_TARGET_PROPERTIES(sylar_static PROPERTIES OUTPUT_NAME "sylar")

set(LIB_LIB
    pthread
    yaml-cpp 
)

# add_executable(test_config tests/test_config.cc)
# #add_dependencies(test_config sylar)
# force_redefine_file_macro_for_sources(test_config)
# target_link_libraries(test_config sylar ${LIB_LIB})

# add_executable(test_thread tests/test_thread.cc)
# #add_dependencies(test_config sylar)
# force_redefine_file_macro_for_sources(test_thread)
# target_link_libraries(test_thread sylar ${LIB_LIB})

# add_executable(test_util tests/test_util.cc)
# #add_dependencies(test_config sylar)
# force_redefine_file_macro_for_sources(test_util)
# target_link_libraries(test_util sylar ${LIB_LIB})

add_executable(test_fiber tests/test_fiber.cc)
#add_dependencies(test_config sylar)
force_redefine_file_macro_for_sources(test_fiber)
target_link_libraries(test_fiber sylar ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)