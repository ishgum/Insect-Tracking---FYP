# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/cmake-gui

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Output

# Include any dependencies generated for this target.
include CMakeFiles/Track_Insect.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Track_Insect.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Track_Insect.dir/flags.make

CMakeFiles/Track_Insect.dir/Main.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/Main.cpp.o: ../Main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Output/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/Main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/Main.cpp.o -c /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Main.cpp

CMakeFiles/Track_Insect.dir/Main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/Main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Main.cpp > CMakeFiles/Track_Insect.dir/Main.cpp.i

CMakeFiles/Track_Insect.dir/Main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/Main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Main.cpp -o CMakeFiles/Track_Insect.dir/Main.cpp.s

CMakeFiles/Track_Insect.dir/Main.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/Main.cpp.o.requires

CMakeFiles/Track_Insect.dir/Main.cpp.o.provides: CMakeFiles/Track_Insect.dir/Main.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/Main.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/Main.cpp.o.provides

CMakeFiles/Track_Insect.dir/Main.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/Main.cpp.o

CMakeFiles/Track_Insect.dir/Insect.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/Insect.cpp.o: ../Insect.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Output/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/Insect.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/Insect.cpp.o -c /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Insect.cpp

CMakeFiles/Track_Insect.dir/Insect.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/Insect.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Insect.cpp > CMakeFiles/Track_Insect.dir/Insect.cpp.i

CMakeFiles/Track_Insect.dir/Insect.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/Insect.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Insect.cpp -o CMakeFiles/Track_Insect.dir/Insect.cpp.s

CMakeFiles/Track_Insect.dir/Insect.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/Insect.cpp.o.requires

CMakeFiles/Track_Insect.dir/Insect.cpp.o.provides: CMakeFiles/Track_Insect.dir/Insect.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/Insect.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/Insect.cpp.o.provides

CMakeFiles/Track_Insect.dir/Insect.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/Insect.cpp.o

CMakeFiles/Track_Insect.dir/Threshold.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/Threshold.cpp.o: ../Threshold.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Output/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/Threshold.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/Threshold.cpp.o -c /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Threshold.cpp

CMakeFiles/Track_Insect.dir/Threshold.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/Threshold.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Threshold.cpp > CMakeFiles/Track_Insect.dir/Threshold.cpp.i

CMakeFiles/Track_Insect.dir/Threshold.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/Threshold.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Threshold.cpp -o CMakeFiles/Track_Insect.dir/Threshold.cpp.s

CMakeFiles/Track_Insect.dir/Threshold.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/Threshold.cpp.o.requires

CMakeFiles/Track_Insect.dir/Threshold.cpp.o.provides: CMakeFiles/Track_Insect.dir/Threshold.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/Threshold.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/Threshold.cpp.o.provides

CMakeFiles/Track_Insect.dir/Threshold.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/Threshold.cpp.o

CMakeFiles/Track_Insect.dir/Fps.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/Fps.cpp.o: ../Fps.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Output/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/Fps.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/Fps.cpp.o -c /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Fps.cpp

CMakeFiles/Track_Insect.dir/Fps.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/Fps.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Fps.cpp > CMakeFiles/Track_Insect.dir/Fps.cpp.i

CMakeFiles/Track_Insect.dir/Fps.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/Fps.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Fps.cpp -o CMakeFiles/Track_Insect.dir/Fps.cpp.s

CMakeFiles/Track_Insect.dir/Fps.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/Fps.cpp.o.requires

CMakeFiles/Track_Insect.dir/Fps.cpp.o.provides: CMakeFiles/Track_Insect.dir/Fps.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/Fps.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/Fps.cpp.o.provides

CMakeFiles/Track_Insect.dir/Fps.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/Fps.cpp.o

CMakeFiles/Track_Insect.dir/IrCam.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/IrCam.cpp.o: ../IrCam.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Output/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/IrCam.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/IrCam.cpp.o -c /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/IrCam.cpp

CMakeFiles/Track_Insect.dir/IrCam.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/IrCam.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/IrCam.cpp > CMakeFiles/Track_Insect.dir/IrCam.cpp.i

CMakeFiles/Track_Insect.dir/IrCam.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/IrCam.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/IrCam.cpp -o CMakeFiles/Track_Insect.dir/IrCam.cpp.s

CMakeFiles/Track_Insect.dir/IrCam.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/IrCam.cpp.o.requires

CMakeFiles/Track_Insect.dir/IrCam.cpp.o.provides: CMakeFiles/Track_Insect.dir/IrCam.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/IrCam.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/IrCam.cpp.o.provides

CMakeFiles/Track_Insect.dir/IrCam.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/IrCam.cpp.o

# Object files for target Track_Insect
Track_Insect_OBJECTS = \
"CMakeFiles/Track_Insect.dir/Main.cpp.o" \
"CMakeFiles/Track_Insect.dir/Insect.cpp.o" \
"CMakeFiles/Track_Insect.dir/Threshold.cpp.o" \
"CMakeFiles/Track_Insect.dir/Fps.cpp.o" \
"CMakeFiles/Track_Insect.dir/IrCam.cpp.o"

# External object files for target Track_Insect
Track_Insect_EXTERNAL_OBJECTS =

Track_Insect: CMakeFiles/Track_Insect.dir/Main.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/Insect.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/Threshold.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/Fps.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/IrCam.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/build.make
Track_Insect: /usr/local/lib/libopencv_videostab.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_video.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_ts.a
Track_Insect: /usr/local/lib/libopencv_superres.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_stitching.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_photo.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_ocl.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_objdetect.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_nonfree.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_ml.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_legacy.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_imgproc.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_highgui.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_gpu.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_flann.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_features2d.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_core.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_contrib.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_calib3d.so.2.4.11
Track_Insect: /usr/lib/libflycapture.so
Track_Insect: /usr/local/lib/libopencv_nonfree.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_ocl.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_gpu.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_photo.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_objdetect.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_legacy.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_video.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_ml.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_calib3d.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_features2d.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_highgui.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_imgproc.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_flann.so.2.4.11
Track_Insect: /usr/local/lib/libopencv_core.so.2.4.11
Track_Insect: CMakeFiles/Track_Insect.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable Track_Insect"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Track_Insect.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Track_Insect.dir/build: Track_Insect
.PHONY : CMakeFiles/Track_Insect.dir/build

CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/Main.cpp.o.requires
CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/Insect.cpp.o.requires
CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/Threshold.cpp.o.requires
CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/Fps.cpp.o.requires
CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/IrCam.cpp.o.requires
.PHONY : CMakeFiles/Track_Insect.dir/requires

CMakeFiles/Track_Insect.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Track_Insect.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Track_Insect.dir/clean

CMakeFiles/Track_Insect.dir/depend:
	cd /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Output && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Output /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Output /home/turtlebot/Documents/InsectTrackingSummerProject/fyp/EmbeddedSystem/Output/CMakeFiles/Track_Insect.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Track_Insect.dir/depend
