

# --------------------------------------------------------------------------
set(IGNORE_FILES_IN_BUILD_DIRECTORY "^${PROJECT_BINARY_DIR}")

# --------------------------------------------------------------------------
# add_cpplint_tests : This macro generates cpplint tests for files in the
#                     given directory.
#
# The function searches for all sources files in the given directory and
# and generates a cpplint tests for each individual file.
macro(add_cpplint_tests _directory)
  # find files in _directory
  file(GLOB_RECURSE _source_files
       RELATIVE ${SMARTPEAK_HOST_DIRECTORY}/src/${_directory}/
       ${SMARTPEAK_HOST_DIRECTORY}/src/${_directory}/*.cpp)

  # add tests
  foreach(_file_to_test ${_source_files})
    string( REGEX MATCH ${DO_NOT_TEST_THESE_FILES_REGEX} _do_not_test ${_file_to_test} )
    string( REGEX MATCH ${IGNORE_FILES_IN_BUILD_DIRECTORY} _is_in_bin_dir ${_file_to_test})
    if(NOT _do_not_test AND NOT _is_in_bin_dir)
      set(_test_name "src/${_directory}/${_file_to_test}_cpplint_test")

      add_test(${_test_name}
        "${PYTHON_EXECUTABLE}"
        "${PROJECT_SOURCE_DIR}/cpplint.py"
        "--verbose=5"
        "${SMARTPEAK_HOST_DIRECTORY}/src/${_directory}/${_file_to_test}")

      set_tests_properties(
        ${_test_name}
        PROPERTIES
        FAIL_REGULAR_EXPRESSION
        "${CPPLINT_FAIL_REGULAR_EXPRESSION}")
    endif()
  endforeach()
endmacro()

# --------------------------------------------------------------------------
# create tests for all files in the individual file groups
add_cpplint_tests("smartpeak")
