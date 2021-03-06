cmake_minimum_required(VERSION 3.5)
project(oppt_cyber)

# set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -std=gnu++11 -pg")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -std=gnu++11 -pg -fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=gnu++11 -DEIGEN_NO_DEBUG -fPIC")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -std=gnu++11 -pg -w -fPIC")

# RelWithDebInfo is the default build type
if (NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE RelWithDebInfo) 
endif()

find_package(oppt)

# Include the CMakeLists of the different plugins
include(${CMAKE_CURRENT_LIST_DIR}/CyberInitialBeliefPlugins/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/CyberTransitionPlugins/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/CyberObservationPlugins/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/CyberRewardPlugins/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/CyberTerminalPlugins/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/CyberHeuristicPlugins/CMakeLists.txt)

# Include yaml-cpp build shared library
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "disable yaml tests")
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "disable yaml tools")
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "disable yaml contrib")
set(DYAML_BUILD_SHARED_LIBS ON CACHE BOOL "build shared library")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/CyberUtils/yaml-cpp)
include_directories(${CMAKE_CURRENT_LIST_DIR}/CyberUtils/yaml-cpp/include)

target_link_libraries(CyberInitialBeliefPlugin PUBLIC yaml-cpp)
target_link_libraries(CyberLDInitialBeliefPlugin PUBLIC yaml-cpp)
target_link_libraries(CyberLDMInitialBeliefPlugin PUBLIC yaml-cpp)
