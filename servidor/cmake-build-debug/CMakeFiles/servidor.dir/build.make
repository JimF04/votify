# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

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
CMAKE_COMMAND = /snap/clion/265/bin/cmake/linux/x64/bin/cmake

# The command to remove a file.
RM = /snap/clion/265/bin/cmake/linux/x64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gabrielvb/Documents/GitHub/votify/servidor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gabrielvb/Documents/GitHub/votify/servidor/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/servidor.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/servidor.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/servidor.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/servidor.dir/flags.make

CMakeFiles/servidor.dir/main.cpp.o: CMakeFiles/servidor.dir/flags.make
CMakeFiles/servidor.dir/main.cpp.o: /home/gabrielvb/Documents/GitHub/votify/servidor/main.cpp
CMakeFiles/servidor.dir/main.cpp.o: CMakeFiles/servidor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/gabrielvb/Documents/GitHub/votify/servidor/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/servidor.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/servidor.dir/main.cpp.o -MF CMakeFiles/servidor.dir/main.cpp.o.d -o CMakeFiles/servidor.dir/main.cpp.o -c /home/gabrielvb/Documents/GitHub/votify/servidor/main.cpp

CMakeFiles/servidor.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/servidor.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabrielvb/Documents/GitHub/votify/servidor/main.cpp > CMakeFiles/servidor.dir/main.cpp.i

CMakeFiles/servidor.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/servidor.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabrielvb/Documents/GitHub/votify/servidor/main.cpp -o CMakeFiles/servidor.dir/main.cpp.s

CMakeFiles/servidor.dir/playlist.cpp.o: CMakeFiles/servidor.dir/flags.make
CMakeFiles/servidor.dir/playlist.cpp.o: /home/gabrielvb/Documents/GitHub/votify/servidor/playlist.cpp
CMakeFiles/servidor.dir/playlist.cpp.o: CMakeFiles/servidor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/gabrielvb/Documents/GitHub/votify/servidor/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/servidor.dir/playlist.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/servidor.dir/playlist.cpp.o -MF CMakeFiles/servidor.dir/playlist.cpp.o.d -o CMakeFiles/servidor.dir/playlist.cpp.o -c /home/gabrielvb/Documents/GitHub/votify/servidor/playlist.cpp

CMakeFiles/servidor.dir/playlist.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/servidor.dir/playlist.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabrielvb/Documents/GitHub/votify/servidor/playlist.cpp > CMakeFiles/servidor.dir/playlist.cpp.i

CMakeFiles/servidor.dir/playlist.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/servidor.dir/playlist.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabrielvb/Documents/GitHub/votify/servidor/playlist.cpp -o CMakeFiles/servidor.dir/playlist.cpp.s

CMakeFiles/servidor.dir/server.cpp.o: CMakeFiles/servidor.dir/flags.make
CMakeFiles/servidor.dir/server.cpp.o: /home/gabrielvb/Documents/GitHub/votify/servidor/server.cpp
CMakeFiles/servidor.dir/server.cpp.o: CMakeFiles/servidor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/gabrielvb/Documents/GitHub/votify/servidor/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/servidor.dir/server.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/servidor.dir/server.cpp.o -MF CMakeFiles/servidor.dir/server.cpp.o.d -o CMakeFiles/servidor.dir/server.cpp.o -c /home/gabrielvb/Documents/GitHub/votify/servidor/server.cpp

CMakeFiles/servidor.dir/server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/servidor.dir/server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabrielvb/Documents/GitHub/votify/servidor/server.cpp > CMakeFiles/servidor.dir/server.cpp.i

CMakeFiles/servidor.dir/server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/servidor.dir/server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabrielvb/Documents/GitHub/votify/servidor/server.cpp -o CMakeFiles/servidor.dir/server.cpp.s

# Object files for target servidor
servidor_OBJECTS = \
"CMakeFiles/servidor.dir/main.cpp.o" \
"CMakeFiles/servidor.dir/playlist.cpp.o" \
"CMakeFiles/servidor.dir/server.cpp.o"

# External object files for target servidor
servidor_EXTERNAL_OBJECTS =

servidor: CMakeFiles/servidor.dir/main.cpp.o
servidor: CMakeFiles/servidor.dir/playlist.cpp.o
servidor: CMakeFiles/servidor.dir/server.cpp.o
servidor: CMakeFiles/servidor.dir/build.make
servidor: /usr/local/lib/libglog.so.0.8.0
servidor: /home/gabrielvb/.vcpkg-clion/vcpkg\ (4)/installed/x64-linux/debug/lib/libINIReader.a
servidor: /usr/lib/x86_64-linux-gnu/libjsoncpp.so.1.9.5
servidor: /home/gabrielvb/.vcpkg-clion/vcpkg\ (4)/installed/x64-linux/debug/lib/libinih.a
servidor: CMakeFiles/servidor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/gabrielvb/Documents/GitHub/votify/servidor/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable servidor"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/servidor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/servidor.dir/build: servidor
.PHONY : CMakeFiles/servidor.dir/build

CMakeFiles/servidor.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/servidor.dir/cmake_clean.cmake
.PHONY : CMakeFiles/servidor.dir/clean

CMakeFiles/servidor.dir/depend:
	cd /home/gabrielvb/Documents/GitHub/votify/servidor/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gabrielvb/Documents/GitHub/votify/servidor /home/gabrielvb/Documents/GitHub/votify/servidor /home/gabrielvb/Documents/GitHub/votify/servidor/cmake-build-debug /home/gabrielvb/Documents/GitHub/votify/servidor/cmake-build-debug /home/gabrielvb/Documents/GitHub/votify/servidor/cmake-build-debug/CMakeFiles/servidor.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/servidor.dir/depend

