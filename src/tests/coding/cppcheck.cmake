

include(CppcheckTargets)

## we use only source files for cppcheck
set(SOURCE_FILE_REGEX "\\.cpp$")

# --------------------------------------------------------------------------
# add_cpp_check_tests : This macro generates cppcheck tests for files in the
#                       given directory.
#
# The function searches for all sources files in the given directory and
# and generates a cppcheck tests for each individual file.
macro(add_cpp_check_tests _directory)
  # find files in _directory
  file(GLOB_RECURSE _source_files
       RELATIVE ${SMARTPEAK_HOST_DIRECTORY}/src/${_directory}/
       ${SMARTPEAK_HOST_DIRECTORY}/src/${_directory}/*.cpp)

  # add tests
  foreach(_file_to_test ${_source_files})
    string( REGEX MATCH ${SOURCE_FILE_REGEX} _is_source_file ${_file_to_test} )
    string( REGEX MATCH ${DO_NOT_TEST_THESE_FILES_REGEX} _do_not_test ${_file_to_test} )

    if(_is_source_file AND NOT _do_not_test)
      set(_test_name "src/${_directory}/${_file_to_test}")
      add_cppcheck_sources(${_test_name}
                           ${SMARTPEAK_HOST_DIRECTORY}/src/${_directory}/${_file_to_test}
                           STYLE
                           PERFORMANCE
                           INLINE_SUPPRESSION
                           FAIL_ON_WARNINGS)
    endif()
  endforeach()
endmacro()

# --------------------------------------------------------------------------
add_cpp_check_tests("smartpeak")
