if (NOT OPENCV_FOUND)
    find_package(OpenCV  REQUIRED)
endif ()
if (NOT Boost_FOUND)
    find_package(Boost  REQUIRED COMPONENTS system thread)
endif ()
if(NOT HUARAY_FOUND)
    set(HUARAY_DIR .)
    find_package(HUARAY REQUIRED)
endif()
aux_source_directory(. HUARAY_SRC)
add_library(HuarayCam SHARED ${HUARAY_SRC})
target_include_directories(HuarayCam PUBLIC ${Boost_INCLUDE_DIRS} ${HUARAY_INCLUDE_DIR} . base)
target_link_libraries(HuarayCam
        CamBase
        ${HUARAY_LIBRARIES}
        )
