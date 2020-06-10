# - Try to find ImPlot lib
#
# Once done this will define
#
#  IMPLOT_FOUND - system has ImPlot lib with correct version
#  IMPLOT_INCLUDE_DIR - the ImPlot include directory

if (IMPLOT_INCLUDE_DIR)
  set(IMPLOT_FOUND true)
else (IMPLOT_INCLUDE_DIR)
  find_path(IMPLOT_INCLUDE_DIR 
	  NAMES implot.h
	  HINTS ${IMPLOT_DIR}
      NO_CMAKE_FIND_ROOT_PATH
    )
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(ImPlot DEFAULT_MSG IMPLOT_INCLUDE_DIR)
  mark_as_advanced(IMPLOT_INCLUDE_DIR)
endif(IMPLOT_INCLUDE_DIR)