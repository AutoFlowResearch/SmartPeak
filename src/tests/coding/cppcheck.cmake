# --------------------------------------------------------------------------
#   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
# --------------------------------------------------------------------------
# Copyright The SmartPeak Team -- Novo Nordisk Foundation 
# Center for Biosustainability, Technical University of Denmark 2018-2021.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
# INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# --------------------------------------------------------------------------
# $Maintainer: Ahmed Khalil $
# $Authors: Douglas McCloskey $
# --------------------------------------------------------------------------

include(CppcheckTargets)

## we use only source files for cppcheck
set(SOURCE_FILE_REGEX "\\.cpp$")
set(IGNORED_FOLDERS "(/(utils|superbuild)_|tools|data)")
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
    string( REGEX MATCH ${IGNORED_FOLDERS} _do_not_test ${_file_to_test} )

    if(_is_source_file AND NOT _do_not_test)
      set(_test_name "src/${_directory}/${_file_to_test}")
      add_cppcheck_sources(
        ${_test_name}
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
