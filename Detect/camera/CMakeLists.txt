cmake_minimum_required(VERSION 3.1)
project(camera)
set(CMAKE_CXX_STANDARD 14)

message("=========================== HITCRT Camera Drvier ===========================")
# CMAKE_SOURCE_DIR 定义了顶级CMakeLists.txt的文件夹 PROJECT_SOURCE_DIR
# 包含最近的project()命令的CMakeLists.txt的文件夹
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
# 指定可用的编译种类
set(CMAKE_CONFIGURATION_TYPES Debug RelWithDebInfo Release)
# 指定编译种类
set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug) 
# set(CMAKE_BUILD_TYPE RelWithDebInfo) 
# 可选编译选项设置
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -pg")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pg")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -Wall -O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Wall -O3")
# 可选测试选项
option(BUILD_TEST "build test samples" ON)

message(STATUS "====Enable Build Huaray Camera====")

find_package(HUARAY REQUIRED)
find_package(OpenCV  REQUIRED)
find_package(Boost  REQUIRED COMPONENTS system thread)

message(STATUS "HUARAY ${HUARAY_INCLUDE_DIR}")

include_directories(
    ${Boost_INCLUDE_DIRS} 
    ${OpenCV_INCLUDE_DIRS}
    ${HUARAY_INCLUDE_DIR} 
    GoogleTest)

add_subdirectory(base)
add_subdirectory(huaray)

if(${BUILD_TEST})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/demos)
endif()