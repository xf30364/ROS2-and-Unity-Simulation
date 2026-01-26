# 设置头文件目录（通用）
set(HUARAY_INCLUDE_DIR /opt/HuarayTech/MVviewer/include)

# 根据架构自动选择库
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")  # 针对 Orin NX, Jetson, ARM64
    message(STATUS "Target: ARM64 (Orin NX)")
    set(LIB_NAMES
        GCBase_gcc485_v3_0 GenApi_gcc485_v3_0
        ImageConvert log4cpp_gcc485_v3_0 log4cpp Log_gcc485_v3_0 MathParser_gcc485_v3_0
        MVSDKGuiQt MVSDK NodeMapData_gcc485_v3_0  
        XmlParser_gcc485_v3_0)
    set(HUARAY_PATH_NAMES
        /opt/HuarayTech/MVviewer/lib
        /opt/HuarayTech/MVviewer/lib/GenICam/bin)
else()  # 默认视为 x86_64
    message(STATUS "Target: x86_64")
    set(LIB_NAMES
        GCBase_gcc421_v3_0 GenApi_gcc421_v3_0
        ImageConvert log4cpp_gcc421_v3_0 log4cpp Log_gcc421_v3_0 MathParser_gcc421_v3_0
        MVSDKGuiQt MVSDK NodeMapData_gcc421_v3_0
        XmlParser_gcc421_v3_0)
    set(HUARAY_PATH_NAMES
        /opt/HuarayTech/MVviewer/lib
        /opt/HuarayTech/MVviewer/lib/GenICam/bin/Linux64_x64)
endif()

# 遍历查找库
set(HUARAY_LIBRARIES "")
foreach(LIB_NAME ${LIB_NAMES})
    find_library(HUARAY_LIBRARY NAMES ${LIB_NAME} PATHS ${HUARAY_PATH_NAMES})
    if(HUARAY_LIBRARY)
        list(APPEND HUARAY_LIBRARIES ${HUARAY_LIBRARY})
        message(STATUS "HUARAY Found: ${HUARAY_LIBRARY}")
    else()
        message(WARNING "Could not find HUARAY library: ${LIB_NAME}")
        if(HUARAY_FIND_REQUIRED)
            message(FATAL_ERROR "HUARAY library ${LIB_NAME} not found but is required.")
        endif()
    endif()
    unset(HUARAY_LIBRARY CACHE)
endforeach()

set(HUARAY_FOUND TRUE)
