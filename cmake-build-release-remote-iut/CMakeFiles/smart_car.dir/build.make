# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /tmp/tmp.yE1kzQPFFA

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.yE1kzQPFFA/cmake-build-release-remote-iut

# Include any dependencies generated for this target.
include CMakeFiles/smart_car.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/smart_car.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/smart_car.dir/flags.make

CMakeFiles/smart_car.dir/main.cpp.o: CMakeFiles/smart_car.dir/flags.make
CMakeFiles/smart_car.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.yE1kzQPFFA/cmake-build-release-remote-iut/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/smart_car.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/smart_car.dir/main.cpp.o -c /tmp/tmp.yE1kzQPFFA/main.cpp

CMakeFiles/smart_car.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/smart_car.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.yE1kzQPFFA/main.cpp > CMakeFiles/smart_car.dir/main.cpp.i

CMakeFiles/smart_car.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/smart_car.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.yE1kzQPFFA/main.cpp -o CMakeFiles/smart_car.dir/main.cpp.s

CMakeFiles/smart_car.dir/controller.cpp.o: CMakeFiles/smart_car.dir/flags.make
CMakeFiles/smart_car.dir/controller.cpp.o: ../controller.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.yE1kzQPFFA/cmake-build-release-remote-iut/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/smart_car.dir/controller.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/smart_car.dir/controller.cpp.o -c /tmp/tmp.yE1kzQPFFA/controller.cpp

CMakeFiles/smart_car.dir/controller.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/smart_car.dir/controller.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.yE1kzQPFFA/controller.cpp > CMakeFiles/smart_car.dir/controller.cpp.i

CMakeFiles/smart_car.dir/controller.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/smart_car.dir/controller.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.yE1kzQPFFA/controller.cpp -o CMakeFiles/smart_car.dir/controller.cpp.s

CMakeFiles/smart_car.dir/libSonar.cpp.o: CMakeFiles/smart_car.dir/flags.make
CMakeFiles/smart_car.dir/libSonar.cpp.o: ../libSonar.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.yE1kzQPFFA/cmake-build-release-remote-iut/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/smart_car.dir/libSonar.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/smart_car.dir/libSonar.cpp.o -c /tmp/tmp.yE1kzQPFFA/libSonar.cpp

CMakeFiles/smart_car.dir/libSonar.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/smart_car.dir/libSonar.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.yE1kzQPFFA/libSonar.cpp > CMakeFiles/smart_car.dir/libSonar.cpp.i

CMakeFiles/smart_car.dir/libSonar.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/smart_car.dir/libSonar.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.yE1kzQPFFA/libSonar.cpp -o CMakeFiles/smart_car.dir/libSonar.cpp.s

# Object files for target smart_car
smart_car_OBJECTS = \
"CMakeFiles/smart_car.dir/main.cpp.o" \
"CMakeFiles/smart_car.dir/controller.cpp.o" \
"CMakeFiles/smart_car.dir/libSonar.cpp.o"

# External object files for target smart_car
smart_car_EXTERNAL_OBJECTS =

smart_car: CMakeFiles/smart_car.dir/main.cpp.o
smart_car: CMakeFiles/smart_car.dir/controller.cpp.o
smart_car: CMakeFiles/smart_car.dir/libSonar.cpp.o
smart_car: CMakeFiles/smart_car.dir/build.make
smart_car: CMakeFiles/smart_car.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.yE1kzQPFFA/cmake-build-release-remote-iut/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable smart_car"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/smart_car.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/smart_car.dir/build: smart_car

.PHONY : CMakeFiles/smart_car.dir/build

CMakeFiles/smart_car.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/smart_car.dir/cmake_clean.cmake
.PHONY : CMakeFiles/smart_car.dir/clean

CMakeFiles/smart_car.dir/depend:
	cd /tmp/tmp.yE1kzQPFFA/cmake-build-release-remote-iut && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.yE1kzQPFFA /tmp/tmp.yE1kzQPFFA /tmp/tmp.yE1kzQPFFA/cmake-build-release-remote-iut /tmp/tmp.yE1kzQPFFA/cmake-build-release-remote-iut /tmp/tmp.yE1kzQPFFA/cmake-build-release-remote-iut/CMakeFiles/smart_car.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/smart_car.dir/depend

