cmake_minimum_required(VERSION 2.8)
project(InsectTracking)
find_package( OpenCV REQUIRED )

set(SOURCE_DIR /home/ucnavi4/fyp)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg -g")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg -g")

include_directories(src/include)
include_directories(/usr/include/flycapture)

file(GLOB SOURCES "src/*.cpp")

add_executable(Track_Insect ${SOURCES})
#set_property(TARGET Track_Insect PROPERTY CXX_STANDARD 11)
#set_property(TARGET Track_Insect PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(Track_Insect ${OpenCV_LIBS} /usr/lib/libflycapture.so ncurses)

