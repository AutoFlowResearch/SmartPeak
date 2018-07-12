

cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project("SmartPeak_tests")

option(ENABLE_STYLE_TESTING "Enables checking of code convention violations (cpplint) and static code analysis (cppchecker). Note that this will disable the regular test system." OFF)
option(ENABLE_CLASS_TESTING "Enables tests for library classes. Should be disabled only on time constraints (e.g. chunking during continuous integration)." ON)

# why is this not called below?
add_subdirectory(class_tests)
#------------------------------------------------------------------------------
# we only test if we have no package target
if("${PACKAGE_TYPE}" STREQUAL "none")
  if(ENABLE_STYLE_TESTING)
    add_subdirectory(coding)
  else()
    ## configure the regular class ..
    if(ENABLE_CLASS_TESTING)
      add_subdirectory(class_tests)
    endif()
  endif(ENABLE_STYLE_TESTING)
endif("${PACKAGE_TYPE}" STREQUAL "none")
