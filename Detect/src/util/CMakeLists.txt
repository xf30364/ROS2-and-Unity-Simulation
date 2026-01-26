AUX_SOURCE_DIRECTORY(. BASIC_SRC)
add_library(Basic SHARED  ${BASIC_SRC})
target_include_directories(Basic PUBLIC . )
target_link_libraries(Basic
        ${Boost_LIBRARIES}
        ${OpenCV_LIBS}
        )