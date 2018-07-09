#------------------------------------------------------------------------------
# This cmake file handles finding external libs for SmartPeak
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# set which library extensions are preferred (we prefer shared libraries)
if(NOT MSVC)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".so;.a")
endif()
if (APPLE)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".dylib;.a")
endif()

#------------------------------------------------------------------------------
# find libs (for linking)
# On Windows:
#   * on windows we need the *.lib versions (dlls alone won't do for linking)
#   * never mix Release/Debug versions of libraries. Leads to strange segfaults,
#     stack corruption etc, due to different runtime libs ...
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Find eigen3
#------------------------------------------------------------------------------
find_package(Eigen3 3.1.0 REQUIRED)

#------------------------------------------------------------------------------
# Done finding contrib libraries
#------------------------------------------------------------------------------
#except for the contrib libs, prefer shared libraries
if(NOT MSVC AND NOT APPLE)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".so;.a")
endif()
