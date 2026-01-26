cmake_minimum_required(VERSION 3.5)
project(aim_nn)
set(CMAKE_CXX_STANDARD 20)

# CMAKE_SOURCE_DIR 定义了顶级CMakeLists.txt的文件夹 PROJECT_SOURCE_DIR
# 包含最近的project()命令的CMakeLists.txt的文件夹
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
# 指定可用的编译种类
set(CMAKE_CONFIGURATION_TYPES Debug RelWithDebInfo Release)
# 指定编译种类
set(CMAKE_BUILD_TYPE Release)
# 可选编译选项设置
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -pg")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pg")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -Wall -O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Wall -O3")
# 设定相机驱动包查找路径，设置完才能查找到HUARAY
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/camera/cmake)

set(CAMERA_DRIVER_INCLUDE_DIRS 
        ${CMAKE_SOURCE_DIR}/camera/base
        ${CMAKE_SOURCE_DIR}/camera/huaray
        )
set(CAMERA_DRIVER_LIB 
        ${CMAKE_SOURCE_DIR}/camera/lib/libHuarayCam.so
        )

find_package(HUARAY REQUIRED)
find_package(OpenCV  REQUIRED)
find_package(Boost  REQUIRED COMPONENTS system thread)
find_package(OpenMP REQUIRED)
find_package(Eigen3 REQUIRED)


# -------------------------- AimAssistNN ----------------------------------
message(STATUS "AimAssistNN_FLAG is ON, using CUDA & TensorRT configuration...")
find_package(CUDA REQUIRED)
set(CMAKE_CUDA_ARCHITECTURES 80) # 设置GPU架构
set(TENSORRT_PATH "/usr/local/TensorRT-10.0.1.6") # 设置为 TensorRT 安装路径
set(CUDA_PATH ${CUDA_TOOLKIT_ROOT_DIR}) # 缓存CUDA路径
add_subdirectory(deploy)
add_definitions(-DMACRO_AIMASSISTNN_FLAG) 
# -------------------------- AimAssistNN ----------------------------------

include_directories( # 基本都要用，就整个工程都include
        ${Boost_INCLUDE_DIRS} 
        ${OpenCV_INCLUDE_DIRS} 
        ${HUARAY_INCLUDE_DIR} 
        ${EIGEN3_INCLUDE_DIRS}
        )



#添加ros2节点的包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cv_bridge REQUIRED)


add_subdirectory(src)

add_executable(aim_nn_demo demo.cpp)

target_include_directories(aim_nn_demo
        PUBLIC
        ${CAMERA_DRIVER_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src/aim_assist_nn
        )

target_link_libraries(aim_nn_demo
        ${CAMERA_DRIVER_LIB}
        armorDetector 
        ${SENSOR_MSGS_LIBRARIES}
        ) 
ament_target_dependencies(aim_nn_demo std_msgs sensor_msgs rclcpp cv_bridge)
    