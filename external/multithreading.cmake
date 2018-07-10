#------------------------------------------------------------------------------
# This cmake files bundles all the multithreading related stuff of the SmartPeak
# build system.

#------------------------------------------------------------------------------
# TBB
#------------------------------------------------------------------------------
set(MT_TBB_INCLUDE_DIR CACHE PATH "Intel Threading Building Blocks 'include' directory.")
set(MT_TBB_LIBRARY_DIR CACHE PATH "Intel Threading Building Blocks libraries directory.")
message(STATUS "Intel TBB: ${MT_ENABLE_TBB}")
if (MT_ENABLE_TBB)
  find_package(TBB)
  if (NOT TBB_FOUND)
    message(FATAL_ERROR "TBB not found but requested.")
  endif()
endif()

if (TBB_FOUND)
  INCLUDE_DIRECTORIES(${TBB_INCLUDE_DIRS})
  add_definitions(/DSMARTPEAK_HAS_TBB)
endif()

#------------------------------------------------------------------------------
# OpenMP
#------------------------------------------------------------------------------
if (MT_ENABLE_OPENMP)
	find_package(OpenMP)
endif()
message(STATUS "OpenMP: ${MT_ENABLE_OPENMP}")

if (OPENMP_FOUND)
  # do NOT use add_definitions() here, because RC.exe on windows will fail
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	if (NOT MSVC)
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}")
	endif()
endif()
