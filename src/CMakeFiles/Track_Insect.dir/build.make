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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ucnavi4/fyp/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ucnavi4/fyp/src

# Include any dependencies generated for this target.
include CMakeFiles/Track_Insect.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Track_Insect.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Track_Insect.dir/flags.make

CMakeFiles/Track_Insect.dir/main.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/main.cpp.o: main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ucnavi4/fyp/src/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/main.cpp.o -c /home/ucnavi4/fyp/src/main.cpp

CMakeFiles/Track_Insect.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ucnavi4/fyp/src/main.cpp > CMakeFiles/Track_Insect.dir/main.cpp.i

CMakeFiles/Track_Insect.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ucnavi4/fyp/src/main.cpp -o CMakeFiles/Track_Insect.dir/main.cpp.s

CMakeFiles/Track_Insect.dir/main.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/main.cpp.o.requires

CMakeFiles/Track_Insect.dir/main.cpp.o.provides: CMakeFiles/Track_Insect.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/main.cpp.o.provides

CMakeFiles/Track_Insect.dir/main.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/main.cpp.o

CMakeFiles/Track_Insect.dir/insect.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/insect.cpp.o: insect.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ucnavi4/fyp/src/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/insect.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/insect.cpp.o -c /home/ucnavi4/fyp/src/insect.cpp

CMakeFiles/Track_Insect.dir/insect.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/insect.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ucnavi4/fyp/src/insect.cpp > CMakeFiles/Track_Insect.dir/insect.cpp.i

CMakeFiles/Track_Insect.dir/insect.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/insect.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ucnavi4/fyp/src/insect.cpp -o CMakeFiles/Track_Insect.dir/insect.cpp.s

CMakeFiles/Track_Insect.dir/insect.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/insect.cpp.o.requires

CMakeFiles/Track_Insect.dir/insect.cpp.o.provides: CMakeFiles/Track_Insect.dir/insect.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/insect.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/insect.cpp.o.provides

CMakeFiles/Track_Insect.dir/insect.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/insect.cpp.o

CMakeFiles/Track_Insect.dir/threshold.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/threshold.cpp.o: threshold.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ucnavi4/fyp/src/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/threshold.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/threshold.cpp.o -c /home/ucnavi4/fyp/src/threshold.cpp

CMakeFiles/Track_Insect.dir/threshold.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/threshold.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ucnavi4/fyp/src/threshold.cpp > CMakeFiles/Track_Insect.dir/threshold.cpp.i

CMakeFiles/Track_Insect.dir/threshold.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/threshold.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ucnavi4/fyp/src/threshold.cpp -o CMakeFiles/Track_Insect.dir/threshold.cpp.s

CMakeFiles/Track_Insect.dir/threshold.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/threshold.cpp.o.requires

CMakeFiles/Track_Insect.dir/threshold.cpp.o.provides: CMakeFiles/Track_Insect.dir/threshold.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/threshold.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/threshold.cpp.o.provides

CMakeFiles/Track_Insect.dir/threshold.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/threshold.cpp.o

CMakeFiles/Track_Insect.dir/camera_interface.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/camera_interface.cpp.o: camera_interface.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ucnavi4/fyp/src/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/camera_interface.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/camera_interface.cpp.o -c /home/ucnavi4/fyp/src/camera_interface.cpp

CMakeFiles/Track_Insect.dir/camera_interface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/camera_interface.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ucnavi4/fyp/src/camera_interface.cpp > CMakeFiles/Track_Insect.dir/camera_interface.cpp.i

CMakeFiles/Track_Insect.dir/camera_interface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/camera_interface.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ucnavi4/fyp/src/camera_interface.cpp -o CMakeFiles/Track_Insect.dir/camera_interface.cpp.s

CMakeFiles/Track_Insect.dir/camera_interface.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/camera_interface.cpp.o.requires

CMakeFiles/Track_Insect.dir/camera_interface.cpp.o.provides: CMakeFiles/Track_Insect.dir/camera_interface.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/camera_interface.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/camera_interface.cpp.o.provides

CMakeFiles/Track_Insect.dir/camera_interface.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/camera_interface.cpp.o

CMakeFiles/Track_Insect.dir/ui_input.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/ui_input.cpp.o: ui_input.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ucnavi4/fyp/src/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/ui_input.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/ui_input.cpp.o -c /home/ucnavi4/fyp/src/ui_input.cpp

CMakeFiles/Track_Insect.dir/ui_input.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/ui_input.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ucnavi4/fyp/src/ui_input.cpp > CMakeFiles/Track_Insect.dir/ui_input.cpp.i

CMakeFiles/Track_Insect.dir/ui_input.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/ui_input.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ucnavi4/fyp/src/ui_input.cpp -o CMakeFiles/Track_Insect.dir/ui_input.cpp.s

CMakeFiles/Track_Insect.dir/ui_input.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/ui_input.cpp.o.requires

CMakeFiles/Track_Insect.dir/ui_input.cpp.o.provides: CMakeFiles/Track_Insect.dir/ui_input.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/ui_input.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/ui_input.cpp.o.provides

CMakeFiles/Track_Insect.dir/ui_input.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/ui_input.cpp.o

CMakeFiles/Track_Insect.dir/ui_output.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/ui_output.cpp.o: ui_output.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ucnavi4/fyp/src/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/ui_output.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/ui_output.cpp.o -c /home/ucnavi4/fyp/src/ui_output.cpp

CMakeFiles/Track_Insect.dir/ui_output.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/ui_output.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ucnavi4/fyp/src/ui_output.cpp > CMakeFiles/Track_Insect.dir/ui_output.cpp.i

CMakeFiles/Track_Insect.dir/ui_output.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/ui_output.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ucnavi4/fyp/src/ui_output.cpp -o CMakeFiles/Track_Insect.dir/ui_output.cpp.s

CMakeFiles/Track_Insect.dir/ui_output.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/ui_output.cpp.o.requires

CMakeFiles/Track_Insect.dir/ui_output.cpp.o.provides: CMakeFiles/Track_Insect.dir/ui_output.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/ui_output.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/ui_output.cpp.o.provides

CMakeFiles/Track_Insect.dir/ui_output.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/ui_output.cpp.o

CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o: mavlink_interface.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ucnavi4/fyp/src/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o -c /home/ucnavi4/fyp/src/mavlink_interface.cpp

CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ucnavi4/fyp/src/mavlink_interface.cpp > CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.i

CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ucnavi4/fyp/src/mavlink_interface.cpp -o CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.s

CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o.requires

CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o.provides: CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o.provides

CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o

CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o: autopilot_interface.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ucnavi4/fyp/src/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o -c /home/ucnavi4/fyp/src/autopilot_interface.cpp

CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ucnavi4/fyp/src/autopilot_interface.cpp > CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.i

CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ucnavi4/fyp/src/autopilot_interface.cpp -o CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.s

CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o.requires

CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o.provides: CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o.provides

CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o

CMakeFiles/Track_Insect.dir/serial_interface.cpp.o: CMakeFiles/Track_Insect.dir/flags.make
CMakeFiles/Track_Insect.dir/serial_interface.cpp.o: serial_interface.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ucnavi4/fyp/src/CMakeFiles $(CMAKE_PROGRESS_9)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Track_Insect.dir/serial_interface.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Track_Insect.dir/serial_interface.cpp.o -c /home/ucnavi4/fyp/src/serial_interface.cpp

CMakeFiles/Track_Insect.dir/serial_interface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Track_Insect.dir/serial_interface.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ucnavi4/fyp/src/serial_interface.cpp > CMakeFiles/Track_Insect.dir/serial_interface.cpp.i

CMakeFiles/Track_Insect.dir/serial_interface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Track_Insect.dir/serial_interface.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ucnavi4/fyp/src/serial_interface.cpp -o CMakeFiles/Track_Insect.dir/serial_interface.cpp.s

CMakeFiles/Track_Insect.dir/serial_interface.cpp.o.requires:
.PHONY : CMakeFiles/Track_Insect.dir/serial_interface.cpp.o.requires

CMakeFiles/Track_Insect.dir/serial_interface.cpp.o.provides: CMakeFiles/Track_Insect.dir/serial_interface.cpp.o.requires
	$(MAKE) -f CMakeFiles/Track_Insect.dir/build.make CMakeFiles/Track_Insect.dir/serial_interface.cpp.o.provides.build
.PHONY : CMakeFiles/Track_Insect.dir/serial_interface.cpp.o.provides

CMakeFiles/Track_Insect.dir/serial_interface.cpp.o.provides.build: CMakeFiles/Track_Insect.dir/serial_interface.cpp.o

# Object files for target Track_Insect
Track_Insect_OBJECTS = \
"CMakeFiles/Track_Insect.dir/main.cpp.o" \
"CMakeFiles/Track_Insect.dir/insect.cpp.o" \
"CMakeFiles/Track_Insect.dir/threshold.cpp.o" \
"CMakeFiles/Track_Insect.dir/camera_interface.cpp.o" \
"CMakeFiles/Track_Insect.dir/ui_input.cpp.o" \
"CMakeFiles/Track_Insect.dir/ui_output.cpp.o" \
"CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o" \
"CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o" \
"CMakeFiles/Track_Insect.dir/serial_interface.cpp.o"

# External object files for target Track_Insect
Track_Insect_EXTERNAL_OBJECTS =

Track_Insect: CMakeFiles/Track_Insect.dir/main.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/insect.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/threshold.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/camera_interface.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/ui_input.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/ui_output.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o
Track_Insect: CMakeFiles/Track_Insect.dir/serial_interface.cpp.o
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

CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/main.cpp.o.requires
CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/insect.cpp.o.requires
CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/threshold.cpp.o.requires
CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/camera_interface.cpp.o.requires
CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/ui_input.cpp.o.requires
CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/ui_output.cpp.o.requires
CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/mavlink_interface.cpp.o.requires
CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/autopilot_interface.cpp.o.requires
CMakeFiles/Track_Insect.dir/requires: CMakeFiles/Track_Insect.dir/serial_interface.cpp.o.requires
.PHONY : CMakeFiles/Track_Insect.dir/requires

CMakeFiles/Track_Insect.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Track_Insect.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Track_Insect.dir/clean

CMakeFiles/Track_Insect.dir/depend:
	cd /home/ucnavi4/fyp/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ucnavi4/fyp/src /home/ucnavi4/fyp/src /home/ucnavi4/fyp/src /home/ucnavi4/fyp/src /home/ucnavi4/fyp/src/CMakeFiles/Track_Insect.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Track_Insect.dir/depend

