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
include samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/depend.make

# Include the progress variables for this target.
include samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/progress.make

# Include the compile flags for this target's objects.
include samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/flags.make

samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/rexxasp3.c.o: samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/flags.make
samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/rexxasp3.c.o: samples/unix/api/wpipe3/rexxasp3.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/rvjansen/apps/oorexx-code-0/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/rexxasp3.c.o"
	cd /Users/rvjansen/apps/oorexx-code-0/samples/unix/api/wpipe3 && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/wpipe3.dir/rexxasp3.c.o   -c /Users/rvjansen/apps/oorexx-code-0/samples/unix/api/wpipe3/rexxasp3.c

samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/rexxasp3.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/wpipe3.dir/rexxasp3.c.i"
	cd /Users/rvjansen/apps/oorexx-code-0/samples/unix/api/wpipe3 && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/rvjansen/apps/oorexx-code-0/samples/unix/api/wpipe3/rexxasp3.c > CMakeFiles/wpipe3.dir/rexxasp3.c.i

samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/rexxasp3.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/wpipe3.dir/rexxasp3.c.s"
	cd /Users/rvjansen/apps/oorexx-code-0/samples/unix/api/wpipe3 && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/rvjansen/apps/oorexx-code-0/samples/unix/api/wpipe3/rexxasp3.c -o CMakeFiles/wpipe3.dir/rexxasp3.c.s

# Object files for target wpipe3
wpipe3_OBJECTS = \
"CMakeFiles/wpipe3.dir/rexxasp3.c.o"

# External object files for target wpipe3
wpipe3_EXTERNAL_OBJECTS =

lib/libwpipe3.dylib: samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/rexxasp3.c.o
lib/libwpipe3.dylib: samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/build.make
lib/libwpipe3.dylib: lib/librexx.5.0.0.dylib
lib/libwpipe3.dylib: lib/librexxapi.5.0.0.dylib
lib/libwpipe3.dylib: samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/rvjansen/apps/oorexx-code-0/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C shared library ../../../../lib/libwpipe3.dylib"
	cd /Users/rvjansen/apps/oorexx-code-0/samples/unix/api/wpipe3 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/wpipe3.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/build: lib/libwpipe3.dylib

.PHONY : samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/build

samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/clean:
	cd /Users/rvjansen/apps/oorexx-code-0/samples/unix/api/wpipe3 && $(CMAKE_COMMAND) -P CMakeFiles/wpipe3.dir/cmake_clean.cmake
.PHONY : samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/clean

samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/depend:
	cd /Users/rvjansen/apps/oorexx-code-0 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0/samples/unix/api/wpipe3 /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0/samples/unix/api/wpipe3 /Users/rvjansen/apps/oorexx-code-0/samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : samples/unix/api/wpipe3/CMakeFiles/wpipe3.dir/depend
