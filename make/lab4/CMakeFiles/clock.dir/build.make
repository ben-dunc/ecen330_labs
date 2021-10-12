# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/lib/python3.6/dist-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /usr/local/lib/python3.6/dist-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /fsb/bendunc/ecen330

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /fsb/bendunc/ecen330/make

# Include any dependencies generated for this target.
include lab4/CMakeFiles/clock.dir/depend.make

# Include the progress variables for this target.
include lab4/CMakeFiles/clock.dir/progress.make

# Include the compile flags for this target's objects.
include lab4/CMakeFiles/clock.dir/flags.make

lab4/CMakeFiles/clock.dir/clockControl.c.o: lab4/CMakeFiles/clock.dir/flags.make
lab4/CMakeFiles/clock.dir/clockControl.c.o: ../lab4/clockControl.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/fsb/bendunc/ecen330/make/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object lab4/CMakeFiles/clock.dir/clockControl.c.o"
	cd /fsb/bendunc/ecen330/make/lab4 && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/clock.dir/clockControl.c.o -c /fsb/bendunc/ecen330/lab4/clockControl.c

lab4/CMakeFiles/clock.dir/clockControl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/clock.dir/clockControl.c.i"
	cd /fsb/bendunc/ecen330/make/lab4 && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /fsb/bendunc/ecen330/lab4/clockControl.c > CMakeFiles/clock.dir/clockControl.c.i

lab4/CMakeFiles/clock.dir/clockControl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/clock.dir/clockControl.c.s"
	cd /fsb/bendunc/ecen330/make/lab4 && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /fsb/bendunc/ecen330/lab4/clockControl.c -o CMakeFiles/clock.dir/clockControl.c.s

lab4/CMakeFiles/clock.dir/clockDisplay.c.o: lab4/CMakeFiles/clock.dir/flags.make
lab4/CMakeFiles/clock.dir/clockDisplay.c.o: ../lab4/clockDisplay.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/fsb/bendunc/ecen330/make/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object lab4/CMakeFiles/clock.dir/clockDisplay.c.o"
	cd /fsb/bendunc/ecen330/make/lab4 && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/clock.dir/clockDisplay.c.o -c /fsb/bendunc/ecen330/lab4/clockDisplay.c

lab4/CMakeFiles/clock.dir/clockDisplay.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/clock.dir/clockDisplay.c.i"
	cd /fsb/bendunc/ecen330/make/lab4 && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /fsb/bendunc/ecen330/lab4/clockDisplay.c > CMakeFiles/clock.dir/clockDisplay.c.i

lab4/CMakeFiles/clock.dir/clockDisplay.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/clock.dir/clockDisplay.c.s"
	cd /fsb/bendunc/ecen330/make/lab4 && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /fsb/bendunc/ecen330/lab4/clockDisplay.c -o CMakeFiles/clock.dir/clockDisplay.c.s

# Object files for target clock
clock_OBJECTS = \
"CMakeFiles/clock.dir/clockControl.c.o" \
"CMakeFiles/clock.dir/clockDisplay.c.o"

# External object files for target clock
clock_EXTERNAL_OBJECTS =

lab4/libclock.a: lab4/CMakeFiles/clock.dir/clockControl.c.o
lab4/libclock.a: lab4/CMakeFiles/clock.dir/clockDisplay.c.o
lab4/libclock.a: lab4/CMakeFiles/clock.dir/build.make
lab4/libclock.a: lab4/CMakeFiles/clock.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/fsb/bendunc/ecen330/make/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library libclock.a"
	cd /fsb/bendunc/ecen330/make/lab4 && $(CMAKE_COMMAND) -P CMakeFiles/clock.dir/cmake_clean_target.cmake
	cd /fsb/bendunc/ecen330/make/lab4 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/clock.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lab4/CMakeFiles/clock.dir/build: lab4/libclock.a

.PHONY : lab4/CMakeFiles/clock.dir/build

lab4/CMakeFiles/clock.dir/clean:
	cd /fsb/bendunc/ecen330/make/lab4 && $(CMAKE_COMMAND) -P CMakeFiles/clock.dir/cmake_clean.cmake
.PHONY : lab4/CMakeFiles/clock.dir/clean

lab4/CMakeFiles/clock.dir/depend:
	cd /fsb/bendunc/ecen330/make && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /fsb/bendunc/ecen330 /fsb/bendunc/ecen330/lab4 /fsb/bendunc/ecen330/make /fsb/bendunc/ecen330/make/lab4 /fsb/bendunc/ecen330/make/lab4/CMakeFiles/clock.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lab4/CMakeFiles/clock.dir/depend

