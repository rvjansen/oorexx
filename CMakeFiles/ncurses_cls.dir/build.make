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

# Utility rule file for ncurses_cls.

# Include the progress variables for this target.
include CMakeFiles/ncurses_cls.dir/progress.make

CMakeFiles/ncurses_cls: bin/ncurses.cls


bin/ncurses.cls: extensions/orxncurses/ncurses.cls
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/rvjansen/apps/oorexx-code-0/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating bin/ncurses.cls"
	/Applications/CMake.app/Contents/bin/cmake -E copy_if_different /Users/rvjansen/apps/oorexx-code-0/extensions/orxncurses/ncurses.cls /Users/rvjansen/apps/oorexx-code-0/bin/ncurses.cls

ncurses_cls: CMakeFiles/ncurses_cls
ncurses_cls: bin/ncurses.cls
ncurses_cls: CMakeFiles/ncurses_cls.dir/build.make

.PHONY : ncurses_cls

# Rule to build all files generated by this target.
CMakeFiles/ncurses_cls.dir/build: ncurses_cls

.PHONY : CMakeFiles/ncurses_cls.dir/build

CMakeFiles/ncurses_cls.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ncurses_cls.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ncurses_cls.dir/clean

CMakeFiles/ncurses_cls.dir/depend:
	cd /Users/rvjansen/apps/oorexx-code-0 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0/CMakeFiles/ncurses_cls.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ncurses_cls.dir/depend

