if (NOT OPENCV_FOUND)
    find_package(OpenCV  REQUIRED)
endif ()
if (NOT Boost_FOUND)
    find_package(Boost  REQUIRED COMPONENTS system thread)
endif ()

aux_source_directory(. CAM_BASE_SRC)
add_library(CamBase SHARED ${CAM_BASE_SRC})
target_include_directories(CamBase PUBLIC ${Boost_INCLUDE_DIRS} ./)

