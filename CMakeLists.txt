cmake_minimum_required(VERSION 3.10)

project(hailo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-base-1.0 gstreamer-video-1.0)

include_directories(
    ${GST_INCLUDE_DIRS}
    /usr/include/
    /usr/include/hailo/tappas
)
link_directories(${GST_LIBRARY_DIRS})

add_executable(hailo-gst 
    src/gstream/main.cpp
)

target_link_libraries(hailo-gst 
    ${GST_LIBRARIES}
)

add_library(hailo-gst-postproc SHARED
    src/gstream/postprocess.cpp
)

add_executable(hailo-direct-infer
    src/direct/main.cpp
)

target_link_libraries(hailo-direct-infer
    hailort
)
