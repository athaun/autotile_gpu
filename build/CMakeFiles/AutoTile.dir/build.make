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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.27.4/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.27.4/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/asher/code/work/autotile_gpu

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/asher/code/work/autotile_gpu/build

# Include any dependencies generated for this target.
include CMakeFiles/AutoTile.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/AutoTile.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/AutoTile.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/AutoTile.dir/flags.make

CMakeFiles/AutoTile.dir/src/main.cpp.o: CMakeFiles/AutoTile.dir/flags.make
CMakeFiles/AutoTile.dir/src/main.cpp.o: /Users/asher/code/work/autotile_gpu/src/main.cpp
CMakeFiles/AutoTile.dir/src/main.cpp.o: CMakeFiles/AutoTile.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/asher/code/work/autotile_gpu/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/AutoTile.dir/src/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/AutoTile.dir/src/main.cpp.o -MF CMakeFiles/AutoTile.dir/src/main.cpp.o.d -o CMakeFiles/AutoTile.dir/src/main.cpp.o -c /Users/asher/code/work/autotile_gpu/src/main.cpp

CMakeFiles/AutoTile.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/AutoTile.dir/src/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/asher/code/work/autotile_gpu/src/main.cpp > CMakeFiles/AutoTile.dir/src/main.cpp.i

CMakeFiles/AutoTile.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/AutoTile.dir/src/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/asher/code/work/autotile_gpu/src/main.cpp -o CMakeFiles/AutoTile.dir/src/main.cpp.s

CMakeFiles/AutoTile.dir/src/rules.cpp.o: CMakeFiles/AutoTile.dir/flags.make
CMakeFiles/AutoTile.dir/src/rules.cpp.o: /Users/asher/code/work/autotile_gpu/src/rules.cpp
CMakeFiles/AutoTile.dir/src/rules.cpp.o: CMakeFiles/AutoTile.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/asher/code/work/autotile_gpu/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/AutoTile.dir/src/rules.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/AutoTile.dir/src/rules.cpp.o -MF CMakeFiles/AutoTile.dir/src/rules.cpp.o.d -o CMakeFiles/AutoTile.dir/src/rules.cpp.o -c /Users/asher/code/work/autotile_gpu/src/rules.cpp

CMakeFiles/AutoTile.dir/src/rules.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/AutoTile.dir/src/rules.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/asher/code/work/autotile_gpu/src/rules.cpp > CMakeFiles/AutoTile.dir/src/rules.cpp.i

CMakeFiles/AutoTile.dir/src/rules.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/AutoTile.dir/src/rules.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/asher/code/work/autotile_gpu/src/rules.cpp -o CMakeFiles/AutoTile.dir/src/rules.cpp.s

CMakeFiles/AutoTile.dir/src/seed.cpp.o: CMakeFiles/AutoTile.dir/flags.make
CMakeFiles/AutoTile.dir/src/seed.cpp.o: /Users/asher/code/work/autotile_gpu/src/seed.cpp
CMakeFiles/AutoTile.dir/src/seed.cpp.o: CMakeFiles/AutoTile.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/asher/code/work/autotile_gpu/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/AutoTile.dir/src/seed.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/AutoTile.dir/src/seed.cpp.o -MF CMakeFiles/AutoTile.dir/src/seed.cpp.o.d -o CMakeFiles/AutoTile.dir/src/seed.cpp.o -c /Users/asher/code/work/autotile_gpu/src/seed.cpp

CMakeFiles/AutoTile.dir/src/seed.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/AutoTile.dir/src/seed.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/asher/code/work/autotile_gpu/src/seed.cpp > CMakeFiles/AutoTile.dir/src/seed.cpp.i

CMakeFiles/AutoTile.dir/src/seed.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/AutoTile.dir/src/seed.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/asher/code/work/autotile_gpu/src/seed.cpp -o CMakeFiles/AutoTile.dir/src/seed.cpp.s

CMakeFiles/AutoTile.dir/src/simulator.cpp.o: CMakeFiles/AutoTile.dir/flags.make
CMakeFiles/AutoTile.dir/src/simulator.cpp.o: /Users/asher/code/work/autotile_gpu/src/simulator.cpp
CMakeFiles/AutoTile.dir/src/simulator.cpp.o: CMakeFiles/AutoTile.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/asher/code/work/autotile_gpu/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/AutoTile.dir/src/simulator.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/AutoTile.dir/src/simulator.cpp.o -MF CMakeFiles/AutoTile.dir/src/simulator.cpp.o.d -o CMakeFiles/AutoTile.dir/src/simulator.cpp.o -c /Users/asher/code/work/autotile_gpu/src/simulator.cpp

CMakeFiles/AutoTile.dir/src/simulator.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/AutoTile.dir/src/simulator.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/asher/code/work/autotile_gpu/src/simulator.cpp > CMakeFiles/AutoTile.dir/src/simulator.cpp.i

CMakeFiles/AutoTile.dir/src/simulator.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/AutoTile.dir/src/simulator.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/asher/code/work/autotile_gpu/src/simulator.cpp -o CMakeFiles/AutoTile.dir/src/simulator.cpp.s

CMakeFiles/AutoTile.dir/src/tile.cpp.o: CMakeFiles/AutoTile.dir/flags.make
CMakeFiles/AutoTile.dir/src/tile.cpp.o: /Users/asher/code/work/autotile_gpu/src/tile.cpp
CMakeFiles/AutoTile.dir/src/tile.cpp.o: CMakeFiles/AutoTile.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/asher/code/work/autotile_gpu/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/AutoTile.dir/src/tile.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/AutoTile.dir/src/tile.cpp.o -MF CMakeFiles/AutoTile.dir/src/tile.cpp.o.d -o CMakeFiles/AutoTile.dir/src/tile.cpp.o -c /Users/asher/code/work/autotile_gpu/src/tile.cpp

CMakeFiles/AutoTile.dir/src/tile.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/AutoTile.dir/src/tile.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/asher/code/work/autotile_gpu/src/tile.cpp > CMakeFiles/AutoTile.dir/src/tile.cpp.i

CMakeFiles/AutoTile.dir/src/tile.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/AutoTile.dir/src/tile.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/asher/code/work/autotile_gpu/src/tile.cpp -o CMakeFiles/AutoTile.dir/src/tile.cpp.s

# Object files for target AutoTile
AutoTile_OBJECTS = \
"CMakeFiles/AutoTile.dir/src/main.cpp.o" \
"CMakeFiles/AutoTile.dir/src/rules.cpp.o" \
"CMakeFiles/AutoTile.dir/src/seed.cpp.o" \
"CMakeFiles/AutoTile.dir/src/simulator.cpp.o" \
"CMakeFiles/AutoTile.dir/src/tile.cpp.o"

# External object files for target AutoTile
AutoTile_EXTERNAL_OBJECTS =

AutoTile: CMakeFiles/AutoTile.dir/src/main.cpp.o
AutoTile: CMakeFiles/AutoTile.dir/src/rules.cpp.o
AutoTile: CMakeFiles/AutoTile.dir/src/seed.cpp.o
AutoTile: CMakeFiles/AutoTile.dir/src/simulator.cpp.o
AutoTile: CMakeFiles/AutoTile.dir/src/tile.cpp.o
AutoTile: CMakeFiles/AutoTile.dir/build.make
AutoTile: kompute_build/src/libkompute.a
AutoTile: /opt/homebrew/lib/libvulkan.dylib
AutoTile: kompute_build/src/logger/libkp_logger.a
AutoTile: _deps/fmt-build/libfmt.a
AutoTile: CMakeFiles/AutoTile.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/asher/code/work/autotile_gpu/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable AutoTile"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/AutoTile.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/AutoTile.dir/build: AutoTile
.PHONY : CMakeFiles/AutoTile.dir/build

CMakeFiles/AutoTile.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/AutoTile.dir/cmake_clean.cmake
.PHONY : CMakeFiles/AutoTile.dir/clean

CMakeFiles/AutoTile.dir/depend:
	cd /Users/asher/code/work/autotile_gpu/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/asher/code/work/autotile_gpu /Users/asher/code/work/autotile_gpu /Users/asher/code/work/autotile_gpu/build /Users/asher/code/work/autotile_gpu/build /Users/asher/code/work/autotile_gpu/build/CMakeFiles/AutoTile.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/AutoTile.dir/depend

