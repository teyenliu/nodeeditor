# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/liudanny/git/nodeeditor_danny/examples/images

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/liudanny/git/nodeeditor_danny/examples/images/build

# Include any dependencies generated for this target.
include CMakeFiles/datamodel.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/datamodel.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/datamodel.dir/flags.make

CMakeFiles/datamodel.dir/dds/HelloWorldData.o: CMakeFiles/datamodel.dir/flags.make
CMakeFiles/datamodel.dir/dds/HelloWorldData.o: ../dds/HelloWorldData.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/liudanny/git/nodeeditor_danny/examples/images/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/datamodel.dir/dds/HelloWorldData.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/datamodel.dir/dds/HelloWorldData.o -c /home/liudanny/git/nodeeditor_danny/examples/images/dds/HelloWorldData.cpp

CMakeFiles/datamodel.dir/dds/HelloWorldData.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/datamodel.dir/dds/HelloWorldData.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/liudanny/git/nodeeditor_danny/examples/images/dds/HelloWorldData.cpp > CMakeFiles/datamodel.dir/dds/HelloWorldData.i

CMakeFiles/datamodel.dir/dds/HelloWorldData.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/datamodel.dir/dds/HelloWorldData.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/liudanny/git/nodeeditor_danny/examples/images/dds/HelloWorldData.cpp -o CMakeFiles/datamodel.dir/dds/HelloWorldData.s

CMakeFiles/datamodel.dir/dds/HelloWorldData.o.requires:

.PHONY : CMakeFiles/datamodel.dir/dds/HelloWorldData.o.requires

CMakeFiles/datamodel.dir/dds/HelloWorldData.o.provides: CMakeFiles/datamodel.dir/dds/HelloWorldData.o.requires
	$(MAKE) -f CMakeFiles/datamodel.dir/build.make CMakeFiles/datamodel.dir/dds/HelloWorldData.o.provides.build
.PHONY : CMakeFiles/datamodel.dir/dds/HelloWorldData.o.provides

CMakeFiles/datamodel.dir/dds/HelloWorldData.o.provides.build: CMakeFiles/datamodel.dir/dds/HelloWorldData.o


CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o: CMakeFiles/datamodel.dir/flags.make
CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o: ../dds/HelloWorldDataSplDcps.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/liudanny/git/nodeeditor_danny/examples/images/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o -c /home/liudanny/git/nodeeditor_danny/examples/images/dds/HelloWorldDataSplDcps.cpp

CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/liudanny/git/nodeeditor_danny/examples/images/dds/HelloWorldDataSplDcps.cpp > CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.i

CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/liudanny/git/nodeeditor_danny/examples/images/dds/HelloWorldDataSplDcps.cpp -o CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.s

CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o.requires:

.PHONY : CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o.requires

CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o.provides: CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o.requires
	$(MAKE) -f CMakeFiles/datamodel.dir/build.make CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o.provides.build
.PHONY : CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o.provides

CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o.provides.build: CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o


CMakeFiles/datamodel.dir/dds/HelloWorldImg.o: CMakeFiles/datamodel.dir/flags.make
CMakeFiles/datamodel.dir/dds/HelloWorldImg.o: ../dds/HelloWorldImg.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/liudanny/git/nodeeditor_danny/examples/images/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/datamodel.dir/dds/HelloWorldImg.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/datamodel.dir/dds/HelloWorldImg.o -c /home/liudanny/git/nodeeditor_danny/examples/images/dds/HelloWorldImg.cpp

CMakeFiles/datamodel.dir/dds/HelloWorldImg.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/datamodel.dir/dds/HelloWorldImg.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/liudanny/git/nodeeditor_danny/examples/images/dds/HelloWorldImg.cpp > CMakeFiles/datamodel.dir/dds/HelloWorldImg.i

CMakeFiles/datamodel.dir/dds/HelloWorldImg.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/datamodel.dir/dds/HelloWorldImg.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/liudanny/git/nodeeditor_danny/examples/images/dds/HelloWorldImg.cpp -o CMakeFiles/datamodel.dir/dds/HelloWorldImg.s

CMakeFiles/datamodel.dir/dds/HelloWorldImg.o.requires:

.PHONY : CMakeFiles/datamodel.dir/dds/HelloWorldImg.o.requires

CMakeFiles/datamodel.dir/dds/HelloWorldImg.o.provides: CMakeFiles/datamodel.dir/dds/HelloWorldImg.o.requires
	$(MAKE) -f CMakeFiles/datamodel.dir/build.make CMakeFiles/datamodel.dir/dds/HelloWorldImg.o.provides.build
.PHONY : CMakeFiles/datamodel.dir/dds/HelloWorldImg.o.provides

CMakeFiles/datamodel.dir/dds/HelloWorldImg.o.provides.build: CMakeFiles/datamodel.dir/dds/HelloWorldImg.o


CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o: CMakeFiles/datamodel.dir/flags.make
CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o: ../dds/HelloWorldImgSplDcps.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/liudanny/git/nodeeditor_danny/examples/images/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o -c /home/liudanny/git/nodeeditor_danny/examples/images/dds/HelloWorldImgSplDcps.cpp

CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/liudanny/git/nodeeditor_danny/examples/images/dds/HelloWorldImgSplDcps.cpp > CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.i

CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/liudanny/git/nodeeditor_danny/examples/images/dds/HelloWorldImgSplDcps.cpp -o CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.s

CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o.requires:

.PHONY : CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o.requires

CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o.provides: CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o.requires
	$(MAKE) -f CMakeFiles/datamodel.dir/build.make CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o.provides.build
.PHONY : CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o.provides

CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o.provides.build: CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o


# Object files for target datamodel
datamodel_OBJECTS = \
"CMakeFiles/datamodel.dir/dds/HelloWorldData.o" \
"CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o" \
"CMakeFiles/datamodel.dir/dds/HelloWorldImg.o" \
"CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o"

# External object files for target datamodel
datamodel_EXTERNAL_OBJECTS =

libdatamodel.so: CMakeFiles/datamodel.dir/dds/HelloWorldData.o
libdatamodel.so: CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o
libdatamodel.so: CMakeFiles/datamodel.dir/dds/HelloWorldImg.o
libdatamodel.so: CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o
libdatamodel.so: CMakeFiles/datamodel.dir/build.make
libdatamodel.so: /home/liudanny/HDE/x86_64.linux/lib/libddskernel.so
libdatamodel.so: /home/liudanny/HDE/x86_64.linux/lib/libdcpsisocpp2.so
libdatamodel.so: /home/liudanny/HDE/x86_64.linux/lib/libdcpssacpp.so
libdatamodel.so: CMakeFiles/datamodel.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/liudanny/git/nodeeditor_danny/examples/images/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX shared library libdatamodel.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/datamodel.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/datamodel.dir/build: libdatamodel.so

.PHONY : CMakeFiles/datamodel.dir/build

CMakeFiles/datamodel.dir/requires: CMakeFiles/datamodel.dir/dds/HelloWorldData.o.requires
CMakeFiles/datamodel.dir/requires: CMakeFiles/datamodel.dir/dds/HelloWorldDataSplDcps.o.requires
CMakeFiles/datamodel.dir/requires: CMakeFiles/datamodel.dir/dds/HelloWorldImg.o.requires
CMakeFiles/datamodel.dir/requires: CMakeFiles/datamodel.dir/dds/HelloWorldImgSplDcps.o.requires

.PHONY : CMakeFiles/datamodel.dir/requires

CMakeFiles/datamodel.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/datamodel.dir/cmake_clean.cmake
.PHONY : CMakeFiles/datamodel.dir/clean

CMakeFiles/datamodel.dir/depend:
	cd /home/liudanny/git/nodeeditor_danny/examples/images/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/liudanny/git/nodeeditor_danny/examples/images /home/liudanny/git/nodeeditor_danny/examples/images /home/liudanny/git/nodeeditor_danny/examples/images/build /home/liudanny/git/nodeeditor_danny/examples/images/build /home/liudanny/git/nodeeditor_danny/examples/images/build/CMakeFiles/datamodel.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/datamodel.dir/depend

