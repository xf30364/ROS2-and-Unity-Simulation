set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
aux_source_directory(. DEMO_SRC)
add_executable(huarayDemo demo.cpp)
target_include_directories(huarayDemo PUBLIC
	../../base
	../../huaray
)
target_link_libraries(huarayDemo
        ${OpenCV_LIBS}
        ${HUARAY_LIBRARIES}
        pthread
        HuarayCam
        )