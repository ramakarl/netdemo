# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /data/Codes/netdemo/ndserver

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /data/Codes/netdemo/ndserver

# Include any dependencies generated for this target.
include CMakeFiles/nd_server.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/nd_server.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/nd_server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/nd_server.dir/flags.make

CMakeFiles/nd_server.dir/netdemo_server.cpp.o: CMakeFiles/nd_server.dir/flags.make
CMakeFiles/nd_server.dir/netdemo_server.cpp.o: netdemo_server.cpp
CMakeFiles/nd_server.dir/netdemo_server.cpp.o: CMakeFiles/nd_server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/Codes/netdemo/ndserver/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/nd_server.dir/netdemo_server.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/nd_server.dir/netdemo_server.cpp.o -MF CMakeFiles/nd_server.dir/netdemo_server.cpp.o.d -o CMakeFiles/nd_server.dir/netdemo_server.cpp.o -c /data/Codes/netdemo/ndserver/netdemo_server.cpp

CMakeFiles/nd_server.dir/netdemo_server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nd_server.dir/netdemo_server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/Codes/netdemo/ndserver/netdemo_server.cpp > CMakeFiles/nd_server.dir/netdemo_server.cpp.i

CMakeFiles/nd_server.dir/netdemo_server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nd_server.dir/netdemo_server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/Codes/netdemo/ndserver/netdemo_server.cpp -o CMakeFiles/nd_server.dir/netdemo_server.cpp.s

# Object files for target nd_server
nd_server_OBJECTS = \
"CMakeFiles/nd_server.dir/netdemo_server.cpp.o"

# External object files for target nd_server
nd_server_EXTERNAL_OBJECTS =

nd_server: CMakeFiles/nd_server.dir/netdemo_server.cpp.o
nd_server: CMakeFiles/nd_server.dir/build.make
nd_server: /data/Codes/build/libmin/bin/
nd_server: CMakeFiles/nd_server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data/Codes/netdemo/ndserver/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable nd_server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/nd_server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/nd_server.dir/build: nd_server
.PHONY : CMakeFiles/nd_server.dir/build

CMakeFiles/nd_server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/nd_server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/nd_server.dir/clean

CMakeFiles/nd_server.dir/depend:
	cd /data/Codes/netdemo/ndserver && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/Codes/netdemo/ndserver /data/Codes/netdemo/ndserver /data/Codes/netdemo/ndserver /data/Codes/netdemo/ndserver /data/Codes/netdemo/ndserver/CMakeFiles/nd_server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/nd_server.dir/depend
