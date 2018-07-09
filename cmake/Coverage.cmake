
#------------------------------------------------------------------------------
# This cmake file enables generation of gcov files.

# add compiler flag
# -> requires clang > 3.1 or gcc > 4.8 (will be checked in module)
if (MSVC)
  message(FATAL_ERROR "Coverage tests with gcov/lcov can only be enabled for GCC or Clang in a UNIX environment")
else()
  # add Coverage also for compiler
  add_definitions( -fprofile-arcs -ftest-coverage )
  # add Coverage also for linker
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
  include(cmake/modules/CodeCoverage.cmake)
  # in addition to system paths, also ignore includes from the prefix path (e.g. contrib libs)
  setup_target_for_coverage(SmartPeak_coverage coverage ${CMAKE_PREFIX_PATH})
  message("Coverage tests enabled. You can run 'make SmartPeak_coverage'") 
endif()


