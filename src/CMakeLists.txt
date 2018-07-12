cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project("SmartPeak_src")

#------------------------------------------------------------------------------
# SmartPeak
#------------------------------------------------------------------------------
add_subdirectory(smartpeak)

#------------------------------------------------------------------------------
# Tests
#------------------------------------------------------------------------------
option (BUILD_TESTS "Whether or not build unit tests" ON)
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
