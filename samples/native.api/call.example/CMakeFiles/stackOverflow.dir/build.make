# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/rvjansen/apps/oorexx-code-0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/rvjansen/apps/oorexx-code-0

# Include any dependencies generated for this target.
include samples/native.api/call.example/CMakeFiles/stackOverflow.dir/depend.make

# Include the progress variables for this target.
include samples/native.api/call.example/CMakeFiles/stackOverflow.dir/progress.make

# Include the compile flags for this target's objects.
include samples/native.api/call.example/CMakeFiles/stackOverflow.dir/flags.make

samples/native.api/call.example/CMakeFiles/stackOverflow.dir/stackOverflow.cpp.o: samples/native.api/call.example/CMakeFiles/stackOverflow.dir/flags.make
samples/native.api/call.example/CMakeFiles/stackOverflow.dir/stackOverflow.cpp.o: samples/native.api/call.example/stackOverflow.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/rvjansen/apps/oorexx-code-0/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object samples/native.api/call.example/CMakeFiles/stackOverflow.dir/stackOverflow.cpp.o"
	cd /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/stackOverflow.dir/stackOverflow.cpp.o -c /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example/stackOverflow.cpp

samples/native.api/call.example/CMakeFiles/stackOverflow.dir/stackOverflow.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/stackOverflow.dir/stackOverflow.cpp.i"
	cd /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example/stackOverflow.cpp > CMakeFiles/stackOverflow.dir/stackOverflow.cpp.i

samples/native.api/call.example/CMakeFiles/stackOverflow.dir/stackOverflow.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/stackOverflow.dir/stackOverflow.cpp.s"
	cd /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example/stackOverflow.cpp -o CMakeFiles/stackOverflow.dir/stackOverflow.cpp.s

# Object files for target stackOverflow
stackOverflow_OBJECTS = \
"CMakeFiles/stackOverflow.dir/stackOverflow.cpp.o"

# External object files for target stackOverflow
stackOverflow_EXTERNAL_OBJECTS =

bin/stackOverflow: samples/native.api/call.example/CMakeFiles/stackOverflow.dir/stackOverflow.cpp.o
bin/stackOverflow: samples/native.api/call.example/CMakeFiles/stackOverflow.dir/build.make
bin/stackOverflow: lib/librexx.5.0.0.dylib
bin/stackOverflow: lib/librexxapi.5.0.0.dylib
bin/stackOverflow: samples/native.api/call.example/CMakeFiles/stackOverflow.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/rvjansen/apps/oorexx-code-0/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../bin/stackOverflow"
	cd /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/stackOverflow.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
samples/native.api/call.example/CMakeFiles/stackOverflow.dir/build: bin/stackOverflow

.PHONY : samples/native.api/call.example/CMakeFiles/stackOverflow.dir/build

samples/native.api/call.example/CMakeFiles/stackOverflow.dir/clean:
	cd /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example && $(CMAKE_COMMAND) -P CMakeFiles/stackOverflow.dir/cmake_clean.cmake
.PHONY : samples/native.api/call.example/CMakeFiles/stackOverflow.dir/clean

samples/native.api/call.example/CMakeFiles/stackOverflow.dir/depend:
	cd /Users/rvjansen/apps/oorexx-code-0 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example/CMakeFiles/stackOverflow.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : samples/native.api/call.example/CMakeFiles/stackOverflow.dir/depend

