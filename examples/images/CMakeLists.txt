file(GLOB_RECURSE CPPS  ./*.cpp ./*.cc)

####OpenSplice####
set(CMAKE_MODULE_PATH "/home/liudanny/git/opensplice-cpp-cmake/cmake")
include (MacroOpenSplice)
include (osmacros)

#set(ENV{OSPL_HOME} "/home/liudanny/git/opensplice/install/HDE/x86_64.linux-dev")
#set(CMAKE_PREFIX_PATH "/home/liudanny/git/opensplice-cpp-cmake/cmake")
set(ENV{OSPL_HOME} "/home/liudanny/HDE/x86_64.linux")
find_package (OpenSplice REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${OpenSplice_INCLUDE_DIRS}
)

SET(outsources
    dds/HelloWorldData.cpp
    dds/HelloWorldData.h
    dds/HelloWorldDataSplDcps.cpp
    dds/HelloWorldData_DCPS.hpp
    dds/HelloWorldDataSplDcps.h
    dds/HelloWorldImg.cpp
    dds/HelloWorldImg.h
    dds/HelloWorldImgSplDcps.cpp
    dds/HelloWorldImg_DCPS.hpp
    dds/HelloWorldImgSplDcps.h
)
set (DATAMODEL datamodel)
add_library (${DATAMODEL} SHARED ${outsources})
#add_definitions (
# ${OpenSplice_DEFINITIONS}
# ${DEFINITIONS}
#)

target_link_libraries (
  ${DATAMODEL}
  ${OpenSplice_LIBRARIES}
)
###################

#set(OpenCV_DIR "/opt/opencv4/lib")
find_package(OpenCV 4.0 REQUIRED)
#find_package(Eigen3 REQUIRED)

set(TF_SOURCE "/home/liudanny/git/tensorflow")
set(TF_LIBS "/usr/local/tf_libs")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC -O3 ")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} " )

#include_directories(${EIGEN3_INCLUDE_DIR})
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories("${TF_LIBS}/include/external/nsync/public")
include_directories("${TF_LIBS}/include")
include_directories("${TF_SOURCE}")
include_directories("${TF_SOURCE}/lite")
include_directories("${TF_SOURCE}/tensorflow/contrib/makefile/downloads/absl")

#TVM Related
SET(HOME_TVM "/home/liudanny/git/tvm")
set(USE_CUDA OFF)
INCLUDE_DIRECTORIES(${HOME_TVM}/include)
INCLUDE_DIRECTORIES(${HOME_TVM}/3rdparty/dmlc-core/include)
INCLUDE_DIRECTORIES(${HOME_TVM}/3rdparty/dlpack/include)
INCLUDE_DIRECTORIES(${HOME_TVM})
#INCLUDE_DIRECTORIES("/usr/local/cuda/include")


add_executable(images ${CPPS} draggableicons.qrc)
include_directories(images PRIVATE "./")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(images nodes Threads::Threads )
TARGET_LINK_LIBRARIES(images nodes -lm -ldl )
#TARGET_LINK_LIBRARIES(images nodes "${TF_LIBS}/lib/libxlaaot.so")
TARGET_LINK_LIBRARIES(images nodes "${TF_LIBS}/lib/libtflite.so")
target_link_libraries(images nodes "${TF_LIBS}/lib/libtensorflow_cc.so")
target_link_libraries(images nodes "${TF_LIBS}/lib/libtensorflow_framework.so")
#TARGET_LINK_LIBRARIES(images nodes "${TF_SOURCE}/bazel-bin/tensorflow/compiler/aot/myaot_resnet50/libtest_graph.a")
target_link_libraries(images nodes ${OpenCV_LIBRARIES} ${OpenCV_LIBS})
target_link_libraries(images nodes ${DATAMODEL} ${OpenSplice_LIBRARIES})




