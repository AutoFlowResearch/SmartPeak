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
# ImGUI SDL and OpenGL dependencies
#------------------------------------------------------------------------------
find_package(SDL2 REQUIRED)

#------------------------------------------------------------------------------
# Find ImGUI
#------------------------------------------------------------------------------
find_package(ImGUI REQUIRED)
if (IMGUI_FOUND)
  message(STATUS "ImGui_INCLUDE_DIR : ${ImGui_INCLUDE_DIR}")
endif (IMGUI_FOUND)

#------------------------------------------------------------------------------
# Find plog
#------------------------------------------------------------------------------
find_package(Plog REQUIRED)

#------------------------------------------------------------------------------
# Find BOOST
#------------------------------------------------------------------------------
find_boost(filesystem)

if(Boost_FOUND)
  message(STATUS "Found Boost version ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}" )
  set(CF_OPENMS_BOOST_VERSION_MAJOR ${Boost_MAJOR_VERSION})
	set(CF_OPENMS_BOOST_VERSION_MINOR ${Boost_MINOR_VERSION})
  set(CF_OPENMS_BOOST_VERSION_SUBMINOR ${Boost_SUBMINOR_VERSION})
	set(CF_OPENMS_BOOST_VERSION ${Boost_VERSION})
else()
  message(FATAL_ERROR "Boost or one of its components not found!")
endif()

#------------------------------------------------------------------------------
# Find OpenMS
#------------------------------------------------------------------------------
find_package(OpenMS REQUIRED)

# check whether the OpenMS package was found
if (OpenMS_FOUND)
  # check if the variable containing the include directories is defined
  if(NOT OpenMS_INCLUDE_DIRECTORIES)
    set(_message "The variable \${OpenMS_INCLUDE_DIRECTORIES} is not defined.")
    set(_message "${_message}This CMakeLists.txt and your build of OpenMS seem incompatible.")
    set(_message "${_message}Please use the latest version from the OpenMS release!")
    message(FATAL_ERROR ${_message})
  endif()

  ## append precompiler macros and compiler flags specific to OpenMS
  ## Warning: this could be harmful to your project. Check this if problems occur.
  ## Also, use this to add your own compiler flags, e.g. for OpenMP support.
  ## e.g. for Visual Studio use /openmp
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPENMS_ADDCXX_FLAGS}")

else(OpenMS_FOUND)
  message(FATAL_ERROR "OpenMSConfig.cmake file not found!")
endif(OpenMS_FOUND)

#------------------------------------------------------------------------------
# Done finding contrib libraries
#------------------------------------------------------------------------------
#except for the contrib libs, prefer shared libraries
if(NOT MSVC AND NOT APPLE)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".so;.a")
endif()
