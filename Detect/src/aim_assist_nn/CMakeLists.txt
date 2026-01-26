AUX_SOURCE_DIRECTORY(. AIM_LIB_SRCS)

add_library(armorDetector SHARED ${AIM_LIB_SRCS})
message(STATUS "==== Checking path: ${PROJECT_SOURCE_DIR}/deploy ====")

message(STATUS "Platform: x86_64")

target_link_libraries(armorDetector 
    Basic
    OpenMP::OpenMP_CXX
    ${TENSORRT_PATH}/lib/libnvinfer.so
    ${TENSORRT_PATH}/lib/libnvinfer_plugin.so
    ${TENSORRT_PATH}/lib/libnvonnxparser.so
    ${CUDA_cudart_LIBRARY}
    deploy
)
target_include_directories(armorDetector 
    PUBLIC
    .
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/util
    ${PROJECT_SOURCE_DIR}/deploy
    ${CUDA_INCLUDE_DIRS} # CUDA 包含目录
    ${TENSORRT_PATH}/include # TensorRT 包含目录
)