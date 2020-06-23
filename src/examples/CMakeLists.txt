

cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)
project("SmartPeak_class_examples_smartpeak")

message(STATUS "building examples...")

#------------------------------------------------------------------------------
# Configure examples file to get the TEST_DATA_PATH into the examples
set(CF_SMARTPEAK_EXAMPLES_DATA_PATH "${PROJECT_SOURCE_DIR}/data/")
set(CONFIGURED_EXAMPLES_CONFIG_H "${PROJECT_BINARY_DIR}/include/SmartPeak/test_config.h")
configure_file(${PROJECT_SOURCE_DIR}/include/SmartPeak/test_config.h.in ${CONFIGURED_EXAMPLES_CONFIG_H})

#------------------------------------------------------------------------------
# get the test executables
include(executables.cmake)

#------------------------------------------------------------------------------
# Include directories for examples
set(SMARTPEAK_EXAMPLES_INTERNAL_INCLUDE_DIRECTORIES "${PROJECT_BINARY_DIR}/include/")
# add SmartPeak directories
set(SMARTPEAK_EXAMPLES_EXTERNAL_INCLUDE_DIRECTORIES "${SmartPeak_INCLUDE_DIRECTORIES}")
include_directories(${SMARTPEAK_EXAMPLES_INTERNAL_INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SMARTPEAK_EXAMPLES_EXTERNAL_INCLUDE_DIRECTORIES})

#------------------------------------------------------------------------------
# OpenMS
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
  ## include directories for OpenMS headers (and contrib)
  #       OpenMS_GUI -> ${OpenMS_GUI_INCLUDE_DIRECTORIES}
  include_directories(${OpenMS_INCLUDE_DIRECTORIES})

  ## append precompiler macros and compiler flags specific to OpenMS
  ## Warning: this could be harmful to your project. Check this if problems occur.
  ## Also, use this to add your own compiler flags, e.g. for OpenMP support.
  ## e.g. for Visual Studio use /openmp
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPENMS_ADDCXX_FLAGS}")

else(OpenMS_FOUND)
  message(FATAL_ERROR "OpenMSConfig.cmake file not found!")
endif(OpenMS_FOUND)

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
# OpenMS QT5 dependencies
#------------------------------------------------------------------------------
find_package(Qt5 COMPONENTS Core Network REQUIRED)

if (NOT Qt5Network_FOUND)
  message(STATUS "QtNetwork module not found!")
  message(FATAL_ERROR "To find a custom Qt installation use: cmake <..more options..> -D QT_QMAKE_EXECUTABLE='<path_to_qmake(.exe)' <src-dir>")
endif()

if (NOT Qt5Core_FOUND)
  message(STATUS "QtCore module not found!")
  message(FATAL_ERROR "To find a custom Qt installation use: cmake <..more options..> -D QT_QMAKE_EXECUTABLE='<path_to_qmake(.exe)' <src-dir>")
endif()

#------------------------------------------------------------------------------
# ImGui SDL and OpenGL dependencies
#------------------------------------------------------------------------------
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

#------------------------------------------------------------------------------
# OpenGL
#------------------------------------------------------------------------------
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

#------------------------------------------------------------------------------
# ImGui
#------------------------------------------------------------------------------
find_package(ImGui COMPONENTS Sources Examples SourcesMiscCpp REQUIRED)
include_directories(${ImGui_INCLUDE_DIR})
if (ImGui_Examples_FOUND)
  message(STATUS "ImGui_Examples_FOUND has been set and found")
else()
  message(STATUS "imgui has not been found")
endif()

#------------------------------------------------------------------------------
# IMPLOT
#------------------------------------------------------------------------------
find_package(ImPlot COMPONENTS Sources REQUIRED)
include_directories(${IMPLOT_INCLUDE_DIR})

#------------------------------------------------------------------------------
# pipeline examples
add_custom_target(PIPELINE_EXAMPLES)
add_dependencies(PIPELINE_EXAMPLES ${pipeline_executables_list})

# interactive examples
# TODO: these will need to be moved into the CPack script for user installation
add_custom_target(INTERACTIVE_EXAMPLES) 
add_dependencies(INTERACTIVE_EXAMPLES ${interactive_executables_list})

#------------------------------------------------------------------------------
# Add the actual examples
foreach(_examples ${EXAMPLE_executables})
  add_executable(${_examples} source/${_examples})
  # message(STATUS "${_examples}")
  if(NOT ${_examples} STREQUAL "GUI")
    target_link_libraries(${_examples} PUBLIC ${SmartPeak_LIBRARIES} OpenMS)
  else()
    target_link_libraries(${_examples} PUBLIC ${SmartPeak_LIBRARIES} OpenMS ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ImPlot::ImPlot ImGui::ImGui PRIVATE ImPlot::Sources ImGui::Sources ImGui::Examples ImGui::SourcesMiscCpp)
	install_tool(${_examples}) # Required by CPack for bundling (see install_macros.cmake and package_general.cmake)
  endif()
  # only add OPENMP flags to gcc linker (execpt Mac OS X, due to compiler bug
  # see https://sourceforge.net/apps/trac/open-ms/ticket/280 for details)
  if (OPENMP_FOUND AND NOT MSVC AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(${_examples} PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})
  endif()
  if (SMARTPEAK_CUDA)
    target_link_libraries(${_examples} ${CUDA_LIBRARIES})
    set_property(TARGET ${_examples} PROPERTY CUDA_STANDARD 11)
  endif()
endforeach(_examples)

#------------------------------------------------------------------------------
# restore old compiler flags
if (CMAKE_COMPILER_IS_INTELCXX OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
  set(${CMAKE_CXX_FLAGS_RELEASE} ${_TMP_CMAKE_CXX_FLAGS_RELEASE})
endif()

#------------------------------------------------------------------------------
# add filenames to Visual Studio solution tree
set(sources_VS)
foreach(i ${EXAMPLE_executables})
  list(APPEND sources_VS "${i}")
endforeach(i)
source_group("" FILES ${sources_VS})
