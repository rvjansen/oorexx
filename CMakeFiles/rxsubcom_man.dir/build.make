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

# Utility rule file for rxsubcom_man.

# Include the progress variables for this target.
include CMakeFiles/rxsubcom_man.dir/progress.make

CMakeFiles/rxsubcom_man: rxsubcom.1.gz


rxsubcom.1.gz: platform/unix/rxsubcom.1
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/rvjansen/apps/oorexx-code-0/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating rxsubcom.1.gz"
	gzip -c /Users/rvjansen/apps/oorexx-code-0/platform/unix/rxsubcom.1 > /Users/rvjansen/apps/oorexx-code-0/bin/rxsubcom.1.gz

rxsubcom_man: CMakeFiles/rxsubcom_man
rxsubcom_man: rxsubcom.1.gz
rxsubcom_man: CMakeFiles/rxsubcom_man.dir/build.make

.PHONY : rxsubcom_man

# Rule to build all files generated by this target.
CMakeFiles/rxsubcom_man.dir/build: rxsubcom_man

.PHONY : CMakeFiles/rxsubcom_man.dir/build

CMakeFiles/rxsubcom_man.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/rxsubcom_man.dir/cmake_clean.cmake
.PHONY : CMakeFiles/rxsubcom_man.dir/clean

CMakeFiles/rxsubcom_man.dir/depend:
	cd /Users/rvjansen/apps/oorexx-code-0 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0 /Users/rvjansen/apps/oorexx-code-0/CMakeFiles/rxsubcom_man.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/rxsubcom_man.dir/depend

