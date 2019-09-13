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
include CMakeFiles/rexxutil.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/rexxutil.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/rexxutil.dir/flags.make

CMakeFiles/rexxutil.dir/extensions/rexxutil/platform/unix/rexxutil.cpp.o: CMakeFiles/rexxutil.dir/flags.make
CMakeFiles/rexxutil.dir/extensions/rexxutil/platform/unix/rexxutil.cpp.o: extensions/rexxutil/platform/unix/rexxutil.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/rvjansen/apps/oorexx-code-0/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/rexxutil.dir/extensions/rexxutil/platform/unix/rexxutil.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/rexxutil.dir/extensions/rexxutil/platform/unix/rexxutil.cpp.o -c /Users/rvjansen/apps/oorexx-code-0/extensions/rexxutil/platform/unix/rexxutil.cpp

CMakeFiles/rexxutil.dir/extensions/rexxutil/platform/unix/rexxutil.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rexxutil.dir/extensions/rexxutil/platform/unix/rexxutil.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/rvjansen/apps/oorexx-code-0/extensions/rexxutil/platform/unix/rexxutil.cpp > CMakeFiles/rexxutil.dir/extensions/rexxutil/platform/unix/rexxutil.cpp.i

CMakeFiles/rexxutil.dir/extensions/rexxutil/platform/unix/rexxutil.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rexxutil.dir/extensions/rexxutil/platform/unix/rexxutil.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/rvjansen/apps/oorexx-code-0/extensions/rexxutil/platform/unix/rexxutil.cpp -o CMakeFiles/rexxutil.dir/extensions/rexxutil/platform/unix/rexxutil.cpp.s

# Object files for target rexxutil
rexxutil_OBJECTS = \
"CMakeFiles/rexxutil.dir/extensions/rexxutil/platform/unix/rexxutil.cpp.o"

# External object files for target rexxutil
rexxutil_EXTERNAL_OBJECTS =

lib/librexxutil.5.0.0.dylib: CMakeFiles/rexxutil.dir/extensions/rexxutil/platform/unix/rexxutil.cpp.o
lib/librexxutil.5.0.0.dylib: CMakeFiles/rexxutil.dir/build.make
lib/librexxutil.5.0.0.dylib: lib/librexx.5.0.0.dylib
lib/librexxutil.5.0.0.dylib: lib/librexxapi.5.0.0.dylib
lib/librexxutil.5.0.0.dylib: CMakeFiles/rexxutil.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/rvjansen/apps/oorexx-code-0/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library lib/librexxutil.dylib"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rexxutil.dir/link.txt --verbose=$(VERBOSE)
	$(CMAKE_COMMAND) -E cmake_symlink_library lib/librexxutil.5.0.0.dylib lib/librexxutil.5.0.0.dylib lib/librexxutil.dylib
	/Applications/CMake.app/Contents/bin/cmake -E chdir /Users/rvjansen/apps/oorexx-code-0/lib ln -sf librexxutil.dylib librexxutil.3.dylib
	/Applications/CMake.app/Contents/bin/cmake -E chdir /Users/rvjansen/apps/oorexx-code-0/lib ln -sf librexxutil.dylib librexxutil.4.dylib

lib/librexxutil.dylib: lib/librexxutil.5.0.0.dylib
	@$(CMAKE_COMMAND) -E touch_nocreate lib/librexxutil.dylib

# Rule to build all files generated by this target.
CMakeFiles/rexxutil.dir/build: lib/librexxutil.dylib

.PHONY : CMakeFiles/rexxutil.dir/build

CMakeFiles/rexxutil.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/rexxutil.dir/cmake_clean.cmake
.PHONY : CMakeFiles/rexxutil.dir/clean

CMakeFiles/rexxutil.dir/depend:
	cd /Users/rvjansen/apps/oorexx-code-0 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0/CMakeFiles/rexxutil.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/rexxutil.dir/depend
