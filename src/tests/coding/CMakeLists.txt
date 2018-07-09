

cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project("SmartPeak_coding_tests")

# --------------------------------------------------------------------------
# cppcheck tests
find_package(cppcheck)
if( CPPCHECK_FOUND )
include( ${PROJECT_SOURCE_DIR}/cppcheck.cmake )
endif(CPPCHECK_FOUND)

# --------------------------------------------------------------------------
# cpplint testing
find_program(PYTHON_EXECUTABLE
         python
         DOC "python executable used to perform coding convention test.")
if("${PYTHON_EXECUTABLE}" STREQUAL "PYTHON_EXECUTABLE-NOTFOUND")
  message(STATUS "Info: The program python could not be found. Coding convention check will not be available! Add the location of python(.exe) to your PATH environment variable.")
else()
  include( ${PROJECT_SOURCE_DIR}/createcpplinttests.cmake )
endif("${PYTHON_EXECUTABLE}" STREQUAL "PYTHON_EXECUTABLE-NOTFOUND")
