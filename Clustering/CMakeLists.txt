cmake_minimum_required (VERSION 3.5)
project(pyClustering)
find_package(pybind11 REQUIRED)

if(DEFINED ENV{CONDA_PREFIX})
  set(CMAKE_PREFIX_PATH "$ENV{CONDA_PREFIX}")
  message(STATUS "Detected Conda environment, CMAKE_PREFIX_PATH set to: ${CMAKE_PREFIX_PATH}")

  execute_process (
    COMMAND bash -c "(python -V 2>&1|awk '{printf $2}'| awk -F. '{printf $2}')"
    OUTPUT_VARIABLE outVar
)

else()
  message(STATUS "No Conda environment detected")
endif()

# set(pybind11_ROOT ${CONDA_PREFIX})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

include(cmake/CheckGit.cmake)
CheckGitSetup()

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

file(GLOB MAC_FILES 
${PROJECT_SOURCE_DIR}/runSingle.py
${PROJECT_SOURCE_DIR}/sugarPosTest.csv
${PROJECT_SOURCE_DIR}/test.csv
${PROJECT_SOURCE_DIR}/testFile.root
)

foreach(_script ${MAC_FILES})
  configure_file(
    ${_script}
    ${PROJECT_BINARY_DIR}/.
    COPYONLY
    )
endforeach()

pybind11_add_module(clustering pyClustering.cc  ${sources} ${headers})
