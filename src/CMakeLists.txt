cmake_minimum_required(VERSION 2.8)
project(InsectTracking)
find_package( OpenCV REQUIRED )
set(SOURCE_DIR /home/ucnavi4/fyp/src/)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -g -lrt")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg -g -lrt")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg -g -lrt")

include_directories(${SOURCE_DIR})
include_directories(/usr/include/flycapture)
set(SOURCES main.cpp insect.cpp threshold.cpp camera_interface.cpp ui_input.cpp ui_output.cpp mavlink_interface.cpp autopilot_interface.cpp serial_interface.cpp)
add_executable(Track_Insect ${SOURCES})
#set_property(TARGET Track_Insect PROPERTY CXX_STANDARD 11)
#set_property(TARGET Track_Insect PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(Track_Insect ${OpenCV_LIBS} /usr/lib/libflycapture.so ncurses)
