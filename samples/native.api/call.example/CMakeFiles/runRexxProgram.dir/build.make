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
include samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/depend.make

# Include the progress variables for this target.
include samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/progress.make

# Include the compile flags for this target's objects.
include samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/flags.make

samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/runRexxProgram.cpp.o: samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/flags.make
samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/runRexxProgram.cpp.o: samples/native.api/call.example/runRexxProgram.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/rvjansen/apps/oorexx-code-0/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/runRexxProgram.cpp.o"
	cd /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/runRexxProgram.dir/runRexxProgram.cpp.o -c /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example/runRexxProgram.cpp

samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/runRexxProgram.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/runRexxProgram.dir/runRexxProgram.cpp.i"
	cd /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example/runRexxProgram.cpp > CMakeFiles/runRexxProgram.dir/runRexxProgram.cpp.i

samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/runRexxProgram.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/runRexxProgram.dir/runRexxProgram.cpp.s"
	cd /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example/runRexxProgram.cpp -o CMakeFiles/runRexxProgram.dir/runRexxProgram.cpp.s

# Object files for target runRexxProgram
runRexxProgram_OBJECTS = \
"CMakeFiles/runRexxProgram.dir/runRexxProgram.cpp.o"

# External object files for target runRexxProgram
runRexxProgram_EXTERNAL_OBJECTS =

bin/runRexxProgram: samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/runRexxProgram.cpp.o
bin/runRexxProgram: samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/build.make
bin/runRexxProgram: lib/librexx.5.0.0.dylib
bin/runRexxProgram: lib/librexxapi.5.0.0.dylib
bin/runRexxProgram: samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/rvjansen/apps/oorexx-code-0/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../bin/runRexxProgram"
	cd /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/runRexxProgram.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/build: bin/runRexxProgram

.PHONY : samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/build

samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/clean:
	cd /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example && $(CMAKE_COMMAND) -P CMakeFiles/runRexxProgram.dir/cmake_clean.cmake
.PHONY : samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/clean

samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/depend:
	cd /Users/rvjansen/apps/oorexx-code-0 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example /Users/rvjansen/apps/oorexx-code-0/samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : samples/native.api/call.example/CMakeFiles/runRexxProgram.dir/depend

