cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project("SmartPeak_src")

#------------------------------------------------------------------------------
# SmartPeak core lib
#------------------------------------------------------------------------------
add_subdirectory(smartpeak)

#------------------------------------------------------------------------------
# Tests
#------------------------------------------------------------------------------
option (BUILD_TESTS "Whether or not build unit tests" ON)
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

#------------------------------------------------------------------------------
# Examples
#------------------------------------------------------------------------------
option (BUILD_EXAMPLES "Whether or not build the examples" ON)
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
