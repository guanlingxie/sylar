# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /home/xitong/sylar/workspace/sylar

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/xitong/sylar/workspace/sylar

# Include any dependencies generated for this target.
include CMakeFiles/test_tcpserver.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_tcpserver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_tcpserver.dir/flags.make

CMakeFiles/test_tcpserver.dir/tests/test_tcpserver.cc.o: CMakeFiles/test_tcpserver.dir/flags.make
CMakeFiles/test_tcpserver.dir/tests/test_tcpserver.cc.o: tests/test_tcpserver.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/xitong/sylar/workspace/sylar/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_tcpserver.dir/tests/test_tcpserver.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"tests/test_tcpserver.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_tcpserver.dir/tests/test_tcpserver.cc.o -c /home/xitong/sylar/workspace/sylar/tests/test_tcpserver.cc

CMakeFiles/test_tcpserver.dir/tests/test_tcpserver.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_tcpserver.dir/tests/test_tcpserver.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"tests/test_tcpserver.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xitong/sylar/workspace/sylar/tests/test_tcpserver.cc > CMakeFiles/test_tcpserver.dir/tests/test_tcpserver.cc.i

CMakeFiles/test_tcpserver.dir/tests/test_tcpserver.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_tcpserver.dir/tests/test_tcpserver.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"tests/test_tcpserver.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xitong/sylar/workspace/sylar/tests/test_tcpserver.cc -o CMakeFiles/test_tcpserver.dir/tests/test_tcpserver.cc.s

# Object files for target test_tcpserver
test_tcpserver_OBJECTS = \
"CMakeFiles/test_tcpserver.dir/tests/test_tcpserver.cc.o"

# External object files for target test_tcpserver
test_tcpserver_EXTERNAL_OBJECTS =

bin/test_tcpserver: CMakeFiles/test_tcpserver.dir/tests/test_tcpserver.cc.o
bin/test_tcpserver: CMakeFiles/test_tcpserver.dir/build.make
bin/test_tcpserver: lib/libsylar.so
bin/test_tcpserver: CMakeFiles/test_tcpserver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/xitong/sylar/workspace/sylar/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/test_tcpserver"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_tcpserver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_tcpserver.dir/build: bin/test_tcpserver

.PHONY : CMakeFiles/test_tcpserver.dir/build

CMakeFiles/test_tcpserver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_tcpserver.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_tcpserver.dir/clean

CMakeFiles/test_tcpserver.dir/depend:
	cd /home/xitong/sylar/workspace/sylar && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/xitong/sylar/workspace/sylar /home/xitong/sylar/workspace/sylar /home/xitong/sylar/workspace/sylar /home/xitong/sylar/workspace/sylar /home/xitong/sylar/workspace/sylar/CMakeFiles/test_tcpserver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_tcpserver.dir/depend

