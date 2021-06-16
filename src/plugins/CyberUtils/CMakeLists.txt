add_executable(ScenarioParser ScenarioParser.cpp)

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "disable yaml tests")
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "disable yaml tools")
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "disable yaml contrib")
set(DYAML_BUILD_SHARED_LIBS ON CACHE BOOL "build shared library")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/yaml-cpp)
include_directories(${CMAKE_CURRENT_LIST_DIR}/yaml-cpp/include)

target_link_libraries(ScenarioParser PUBLIC yaml-cpp)