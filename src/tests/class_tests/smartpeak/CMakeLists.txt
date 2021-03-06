

cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)
project("SmartPeak_class_tests_smartpeak")

#------------------------------------------------------------------------------
# Configure test file to get the TEST_DATA_PATH into the tests
set(CF_SMARTPEAK_TEST_DATA_PATH "${PROJECT_SOURCE_DIR}/data/")
set (CONFIGURED_TEST_CONFIG_H "${PROJECT_BINARY_DIR}/include/SmartPeak/test_config.h")
configure_file(${PROJECT_SOURCE_DIR}/include/SmartPeak/test_config.h.in ${CONFIGURED_TEST_CONFIG_H})

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
# OpenMS QT5 dependencies
#------------------------------------------------------------------------------
find_package(Qt5 COMPONENTS Core Network REQUIRED)

if (NOT Qt5Network_FOUND)
  message(STATUS "[class_tests] : QtNetwork module not found!")
  message(FATAL_ERROR "To find a custom Qt installation use: cmake <..more options..> -D QT_QMAKE_EXECUTABLE='<path_to_qmake(.exe)' <src-dir>")
endif()

if (NOT Qt5Core_FOUND)
  message(STATUS "[class_tests] : QtCore module not found!")
  message(FATAL_ERROR "To find a custom Qt installation use: cmake <..more options..> -D QT_QMAKE_EXECUTABLE='<path_to_qmake(.exe)' <src-dir>")
endif()

#------------------------------------------------------------------------------
# get the test executables
include(executables.cmake)

#------------------------------------------------------------------------------
# googletest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.10.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(FetchContent)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(
    ${googletest_SOURCE_DIR}
    ${googletest_BINARY_DIR}
  )
endif()
include(GoogleTest)

#------------------------------------------------------------------------------
# Include directories for tests
set(SMARTPEAK_CLASS_TESTS_INTERNAL_INCLUDE_DIRECTORIES "${PROJECT_BINARY_DIR}/include/")
# add SmartPeak directories
set(SMARTPEAK_CLASS_TESTS_EXTERNAL_INCLUDE_DIRECTORIES "${SmartPeak_INCLUDE_DIRECTORIES}")
include_directories(${SMARTPEAK_CLASS_TESTS_INTERNAL_INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SMARTPEAK_CLASS_TESTS_EXTERNAL_INCLUDE_DIRECTORIES})

#------------------------------------------------------------------------------
# OpenGL
#------------------------------------------------------------------------------
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

#------------------------------------------------------------------------------
# ImGUI
#------------------------------------------------------------------------------
find_package(ImGui COMPONENTS Sources Examples SourcesMiscCpp REQUIRED)
include_directories(${ImGui_INCLUDE_DIR})
if (ImGui_Examples_FOUND)
  message(STATUS "[class_tests] : ImGui_Examples_FOUND has been set and found")
else()
  message(STATUS "[class_tests] : ImGui has not been found")
endif()

#------------------------------------------------------------------------------
# IMPLOT
#------------------------------------------------------------------------------
find_package(ImPlot COMPONENTS Sources REQUIRED)
include_directories(${IMPLOT_INCLUDE_DIR})
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# core
add_custom_target(CORE_TEST)
add_dependencies(CORE_TEST ${core_executables_list})

# io
add_custom_target(IO_TEST)
add_dependencies(IO_TEST ${io_executables_list})

# algorithm
add_custom_target(ALGORITHM_TEST)
add_dependencies(ALGORITHM_TEST ${algorithm_executables_list})

# ui
# add_custom_target(UI_TEST)
# add_dependencies(UI_TEST ${ui_executables_list})

#------------------------------------------------------------------------------
# Add the actual tests
enable_testing()
foreach(_class_test ${TEST_executables})
  add_executable(${_class_test} source/${_class_test})
  # message(STATUS "${_class_test}")
  if(NOT (${_class_test} STREQUAL "GuiAppender_test" OR ${_class_test} STREQUAL "AppWindow_test" OR ${_class_test} STREQUAL "Widget_test" OR ${_class_test} STREQUAL "ImEntry_test"))
    target_link_libraries(${_class_test} PUBLIC ${SmartPeak_LIBRARIES} OpenMS PRIVATE gtest_main gmock_main)
  else()
    target_link_libraries(${_class_test} PUBLIC ${SmartPeak_LIBRARIES} OpenMS  ${OPENGL_LIBRARIES} ImGui::ImGui ImPlot::ImPlot 
                            PRIVATE gtest_main gmock_main ImGui::Sources ImPlot::Sources ImGui::SourcesMiscCpp)
  endif()
  add_test(${_class_test} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_class_test})
  # only add OPENMP flags to gcc linker (execpt Mac OS X, due to compiler bug
  # see https://sourceforge.net/apps/trac/open-ms/ticket/280 for details)
  if (OPENMP_FOUND AND NOT MSVC AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(${_class_test} PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})
  endif()
  if (EIGEN_USE_GPU)
    target_link_libraries(${_class_test} ${CUDA_LIBRARIES})
    set_property(TARGET ${_class_test} PROPERTY CUDA_STANDARD 11)
  endif()
endforeach(_class_test)

#------------------------------------------------------------------------------
# restore old compiler flags
if (CMAKE_COMPILER_IS_INTELCXX OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
  set(${CMAKE_CXX_FLAGS_RELEASE} ${_TMP_CMAKE_CXX_FLAGS_RELEASE})
endif()

#------------------------------------------------------------------------------
# add filenames to Visual Studio solution tree
set(sources_VS)
foreach(i ${TEST_executables})
  list(APPEND sources_VS "${i}")
endforeach(i)
source_group("" FILES ${sources_VS})
