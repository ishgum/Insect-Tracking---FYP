cmake_minimum_required(VERSION 2.8)
project(InsectTracking)
find_package( OpenCV REQUIRED )
set(SOURCE_DIR /home/ubuntu/fyp/Project2_NT/Project2)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg -g")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg -g")

include_directories(${SOURCE_DIR})
include_directories(/usr/include/flycapture)
set(SOURCES Main.cpp Insect.cpp Thresholding.cpp Fps.cpp ${SOURCE_DIR}/Insect.h ${SOURCE_DIR}/Thresholding.h ${SOURCE_DIR}/Fps.h)
add_executable(Track_Insect ${SOURCES})
#set_property(TARGET Track_Insect PROPERTY CXX_STANDARD 11)
#set_property(TARGET Track_Insect PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(Track_Insect ${OpenCV_LIBS} /usr/lib/libflycapture.so)
