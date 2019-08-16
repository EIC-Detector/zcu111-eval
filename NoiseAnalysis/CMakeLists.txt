# $Id: CMakeLists.txt Baranyai Dávid | 2018.06.05 18:52:01 $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(read_data)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#----------------------------------------------------------------------------
# Find ROOT (required package)
#
find_package(ROOT REQUIRED)

#----------------------------------------------------------------------------
# Setup ROOT include directories and compile definitions
#
include(${ROOT_USE_FILE})

# Setup include directory for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIRS})

set(ROOT_LIBRARIES -L${ROOT_LIBRARY_DIR} -lCore -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

configure_file(serialize_lvm.py serialize_lvm.py COPYONLY)
configure_file(run.sh run.sh COPYONLY)

#----------------------------------------------------------------------------
# Add the executable
#
add_executable(read_data read_data.cc ${sources} ${headers})
add_executable(read_pedestals read_pedestals.cc ${sources} ${headers})
target_link_libraries(read_data ${ROOT_LIBRARIES} )
target_link_libraries(read_pedestals ${ROOT_LIBRARIES} )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS read_data DESTINATION bin)
install(TARGETS read_pedestals DESTINATION bin)
