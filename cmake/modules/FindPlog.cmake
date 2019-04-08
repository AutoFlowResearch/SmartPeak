# - Try to find Plog lib
#
# This module supports requiring a minimum version, e.g. you can do
#   find_package(Plog 3.1.2)
# to require version 3.1.2 or newer of Plog.
#
# Once done this will define
#
#  PLOG_FOUND - system has eigen lib with correct version
#  PLOG_INCLUDE_DIR - the eigen include directory

if (PLOG_INCLUDE_DIR)

  # in cache already
  set(PLOG_FOUND)

else (PLOG_INCLUDE_DIR)

  find_path(PLOG_INCLUDE_DIR NAMES plog
      PATHS
      ${CMAKE_INSTALL_PREFIX}/include
      PATH_SUFFIXES plog
  )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Plog DEFAULT_MSG PLOG_INCLUDE_DIR)

  mark_as_advanced(PLOG_INCLUDE_DIR)

endif(PLOG_INCLUDE_DIR)
