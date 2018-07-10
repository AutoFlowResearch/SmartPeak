
#------------------------------------------------------------------------------
# This cmake file enables the STL debug mode

if (CMAKE_COMPILER_IS_GNUCXX)
	if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		# add compiler flag
  	add_definitions(/D_GLIBCXX_DEBUG)
  	message(STATUS "STL debug mode: ${STL_DEBUG}")
  else()
    message(WARNING "STL debug mode is supported for SmartPeak debug mode only")
  endif()
else()
  message(WARNING "STL debug mode is supported for compiler GCC only")
endif()
