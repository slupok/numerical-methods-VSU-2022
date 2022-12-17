cmake_minimum_required(VERSION 3.5)

project(elastic VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD            17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS         OFF)
set(CMAKE_AUTOMOC                 ON)

set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(EIGEN_DIR ${LIB_DIR}/eigen)
include_directories(${EIGEN_DIR})

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HDRS ${PROJECT_SOURCE_DIR}/src/*.h)

add_executable(${CMAKE_PROJECT_NAME} ${HDRS} ${SRCS})

find_package(Qt5 REQUIRED COMPONENTS Widgets)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt5::Widgets)
