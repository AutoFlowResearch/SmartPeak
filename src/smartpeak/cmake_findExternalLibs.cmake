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
# Find plog
#------------------------------------------------------------------------------
find_package(Plog REQUIRED)

#------------------------------------------------------------------------------
# Find SQLite
#------------------------------------------------------------------------------
find_package(SQLite3 REQUIRED)

#------------------------------------------------------------------------------
# Find OpenMS
#------------------------------------------------------------------------------
find_package(OpenMS REQUIRED)

#------------------------------------------------------------------------------
# Find OpenSSL
#------------------------------------------------------------------------------
find_package(OpenSSL REQUIRED) 
if(OpenSSL_FOUND)
  include_directories(${OPENSSL_INCLUDE_DIR})
  link_directories(${OPENSSL_LIBRARIES})
  message(STATUS "[libSmartPeak] : Using OpenSSL ${OPENSSL_VERSION}")
endif()

#------------------------------------------------------------------------------
# Done finding contrib libraries
#------------------------------------------------------------------------------
#except for the contrib libs, prefer shared libraries
if(NOT MSVC AND NOT APPLE)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".so;.a")
endif()