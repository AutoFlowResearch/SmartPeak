

cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)
project("SmartPeak_class_examples_smartpeak")

message(STATUS "building examples...")

#------------------------------------------------------------------------------
# get the test executables
include(executables.cmake)

#------------------------------------------------------------------------------
# Include directories for examples
set(SMARTPEAK_EXAMPLES_INTERNAL_INCLUDE_DIRECTORIES "${PROJECT_BINARY_DIR}/include/")
# add SmartPeak directories
set(SMARTPEAK_EXAMPLES_EXTERNAL_INCLUDE_DIRECTORIES "${SmartPeak_INCLUDE_DIRECTORIES}")
include_directories(${SMARTPEAK_EXAMPLES_INTERNAL_INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SMARTPEAK_EXAMPLES_EXTERNAL_INCLUDE_DIRECTORIES})

#------------------------------------------------------------------------------
# disable optimization for examples for gcc like compilers
if (CMAKE_COMPILER_IS_INTELCXX OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
  set(_TMP_CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
  set(CMAKE_CXX_FLAGS_RELEASE "-O0")
endif()

#------------------------------------------------------------------------------
# ml examples
add_custom_target(ML_EXAMPLES)
add_dependencies(ML_EXAMPLES ${ml_executables_list})

# cuda examples
add_custom_target(CUDA_EXAMPLES)
add_dependencies(CUDA_EXAMPLES ${cuda_executables_list})

#------------------------------------------------------------------------------
# Add the actual examples
foreach(_examples ${EXAMPLE_executables})
  add_executable(${_examples} source/${_examples})
  target_link_libraries(${_examples} ${SmartPeak_LIBRARIES})
  # only add OPENMP flags to gcc linker (execpt Mac OS X, due to compiler bug
  # see https://sourceforge.net/apps/trac/open-ms/ticket/280 for details)
  if (OPENMP_FOUND AND NOT MSVC AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(${_examples} PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})
  endif()
  if (EVONET_CUDA)
    target_link_libraries(${_examples} ${CUDA_LIBRARIES})
    set_property(TARGET ${_examples} PROPERTY CUDA_STANDARD 11)
  endif()
endforeach(_examples)

#------------------------------------------------------------------------------
# restore old compiler flags
if (CMAKE_COMPILER_IS_INTELCXX OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
  set(${CMAKE_CXX_FLAGS_RELEASE} ${_TMP_CMAKE_CXX_FLAGS_RELEASE})
endif()

#------------------------------------------------------------------------------
# add filenames to Visual Studio solution tree
set(sources_VS)
foreach(i ${EXAMPLE_executables})
  list(APPEND sources_VS "${i}")
endforeach(i)
source_group("" FILES ${sources_VS})
