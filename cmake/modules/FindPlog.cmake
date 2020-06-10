# - Try to find Plog lib
#
# Once done this will define
#
#  PLOG_FOUND - system has plog lib with correct version
#  PLOG_INCLUDE_DIR - the plog include directory

if (PLOG_INCLUDE_DIR)

  # in cache already
  set(PLOG_FOUND true)

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
