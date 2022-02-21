# --------------------------------------------------------------------------
#   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
# --------------------------------------------------------------------------
# Copyright The SmartPeak Team -- Novo Nordisk Foundation 
# Center for Biosustainability, Technical University of Denmark 2018-2022.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
# INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# --------------------------------------------------------------------------
# $Maintainer: Ahmed Khalil, Bertrand Boudaud $
# $Authors: Douglas McCloskey $
# --------------------------------------------------------------------------

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
set(SMARTPEAK_SERVICES_INTERNAL_INCLUDE_DIRECTORIES "${PROJECT_BINARY_DIR}/source/service/")
# add SmartPeak directories
set(SMARTPEAK_EXAMPLES_EXTERNAL_INCLUDE_DIRECTORIES "${SmartPeak_INCLUDE_DIRECTORIES}" "${SmartPeakWidgets_INCLUDE_DIRECTORIES}")
include_directories(${SMARTPEAK_EXAMPLES_INTERNAL_INCLUDE_DIRECTORIES})
include_directories(${SMARTPEAK_SERVICES_INTERNAL_INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SMARTPEAK_EXAMPLES_EXTERNAL_INCLUDE_DIRECTORIES})

#------------------------------------------------------------------------------
# OpenMS
#------------------------------------------------------------------------------
find_package(OpenMS REQUIRED)

#------------------------------------------------------------------------------
# OpenMS QT5 dependencies
#------------------------------------------------------------------------------
find_package(Qt5 COMPONENTS Core Network Sql REQUIRED)

if (NOT Qt5Network_FOUND)
  message(STATUS "[examples] : QtNetwork module not found!")
  message(FATAL_ERROR "To find a custom Qt installation use: cmake <..more options..> -D QT_QMAKE_EXECUTABLE='<path_to_qmake(.exe)' <src-dir>")
endif()

if (NOT Qt5Core_FOUND)
  message(STATUS "[examples] : QtCore module not found!")
  message(FATAL_ERROR "To find a custom Qt installation use: cmake <..more options..> -D QT_QMAKE_EXECUTABLE='<path_to_qmake(.exe)' <src-dir>")
endif()

if (NOT Qt5Sql_FOUND)
  message(STATUS "[examples] : QtSql module not found!")
  message(FATAL_ERROR "To find a custom Qt installation use: cmake <..more options..> -D QT_QMAKE_EXECUTABLE='<path_to_qmake(.exe)' <src-dir>")
endif()

#------------------------------------------------------------------------------
# ImGui SDL and OpenGL dependencies
#------------------------------------------------------------------------------
set(SDL_BUILDING_LIBRARY true)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

#------------------------------------------------------------------------------
# OpenGL
#------------------------------------------------------------------------------
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

#------------------------------------------------------------------------------
# CoreFoundation & ApplicationServices
#------------------------------------------------------------------------------
if(APPLE)
  find_library(COREFOUNDATION_LIBRARY CoreFoundation)
  find_library(APPLICATIONSERVICES_LIBRARY ApplicationServices)
endif()

#------------------------------------------------------------------------------
# ImGui
#------------------------------------------------------------------------------
find_package(ImGui COMPONENTS Sources Examples SourcesMiscCpp REQUIRED)
include_directories(${ImGui_INCLUDE_DIR})
if (ImGui_Examples_FOUND)
  message(STATUS "[examples] : ImGui_Examples_FOUND has been set and found")
else()
  message(STATUS "[examples] : ImGui has not been found")
endif()

#------------------------------------------------------------------------------
# IMPLOT
#------------------------------------------------------------------------------
find_package(ImPlot COMPONENTS Sources REQUIRED)
include_directories(${IMPLOT_INCLUDE_DIR})

#------------------------------------------------------------------------------
# gRPC
#------------------------------------------------------------------------------
include(../../cmake/fetch_grpc.cmake)
#------------------------------------------------------------------------------

find_package(OpenSSL REQUIRED) 
if(OpenSSL_FOUND)
  include_directories(${OPENSSL_INCLUDE_DIR})
  message(">> OPENSSL_INCLUDE_DIR : ${OPENSSL_INCLUDE_DIR}")
  link_directories(${OPENSSL_LIBRARIES})
  message(STATUS "[examples] : Using OpenSSL ${OPENSSL_VERSION}")
endif()

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
  if (APPLE)
    if (${_examples} STREQUAL "SmartPeakGUI")
      set(ASSETS ${PROJECT_SOURCE_DIR}/../../cmake/MacOSX/sp.icns)
      set_source_files_properties(${ASSETS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
      source_group("Resources" FILES ${ASSETS})
      add_executable(${_examples} MACOSX_BUNDLE source/${_examples} ${ASSETS})
      target_link_libraries(${_examples} PUBLIC ${SmartPeakWidgets_LIBRARIES} ${SmartPeak_LIBRARIES} 
            OpenMS ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ImPlot::ImPlot ImGui::ImGui 
            ${COREFOUNDATION_LIBRARY} ${APPLICATIONSERVICES_LIBRARY}
            PRIVATE ImPlot::Sources ImGui::Sources ImGui::Examples ImGui::SourcesMiscCpp
            ${grpc_required_libs} OpenSSL::SSL OpenSSL::Crypto)
      install(TARGETS ${_examples}  BUNDLE DESTINATION .  COMPONENT Applications)
      target_include_directories(${_examples} SYSTEM INTERFACE  ${CMAKE_CURRENT_SOURCE_DIR}/source/service/)
    elseif(${_examples} STREQUAL "SmartPeakServer")
      add_executable(${_examples} source/${_examples})
      target_link_libraries(${_examples} PUBLIC ${SmartPeak_LIBRARIES} OpenMS PRIVATE ${grpc_required_libs})
      target_include_directories(${_examples} SYSTEM INTERFACE  ${CMAKE_CURRENT_SOURCE_DIR}/source/service/)
    else()
      add_executable(${_examples} source/${_examples})
      target_link_libraries(${_examples} PUBLIC ${SmartPeak_LIBRARIES} OpenMS)
    endif()

  elseif(UNIX AND CMAKE_SYSTEM_NAME MATCHES Linux) 

    if (${_examples} STREQUAL "SmartPeakGUI")
      add_executable(${_examples} source/${_examples})
      target_link_libraries(${_examples} PUBLIC ${SmartPeakWidgets_LIBRARIES} ${SmartPeak_LIBRARIES} 
          OpenMS ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ImPlot::ImPlot ImGui::ImGui 
          PRIVATE ImPlot::Sources ImGui::Sources ImGui::Examples ImGui::SourcesMiscCpp
					${grpc_required_libs})
      install_tool(${_examples}) 
    
      file(GLOB_RECURSE OpenMSLibs ${OpenMS_DIR}/lib/*.so)
      install(FILES ${OpenMSLibs} DESTINATION lib)

      if (EXISTS ${OpenMS_DIR}/share)
        set(OpenMS_SHARE_DIR ${OpenMS_DIR}/share/OpenMS/)
      elseif(EXISTS ${OpenMS_DIR}/../share)
        set(OpenMS_SHARE_DIR ${OpenMS_DIR}/../share/OpenMS/)
      endif()
      install(DIRECTORY ${OpenMS_SHARE_DIR} DESTINATION share/OpenMS COMPONENT share)
    elseif(${_examples} STREQUAL "SmartPeakCLI")
      add_executable(${_examples} source/${_examples})
      target_link_libraries(${_examples} PUBLIC ${SmartPeak_LIBRARIES} OpenMS)
      install_tool(${_examples}) 
    elseif(${_examples} STREQUAL "SmartPeakServer")
      add_executable(${_examples} source/${_examples})
      target_link_libraries(${_examples} PUBLIC ${SmartPeak_LIBRARIES} OpenMS PRIVATE ${grpc_required_libs})
      target_include_directories(${_examples} SYSTEM INTERFACE  ${CMAKE_CURRENT_SOURCE_DIR}/source/service/)
    else()
      add_executable(${_examples} source/${_examples})
      target_link_libraries(${_examples} PUBLIC ${SmartPeak_LIBRARIES} OpenMS ${grpc_required_libs})
    endif()

  else() # Win

    if(${_examples} STREQUAL "SmartPeakGUI")
      add_executable(${_examples} source/${_examples} ${CMAKE_SOURCE_DIR}/cmake/Windows/SmartPeakGUI.rc)
      target_link_libraries(${_examples} PUBLIC ${SmartPeakWidgets_LIBRARIES} ${SmartPeak_LIBRARIES} 
        OpenMS ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ImPlot::ImPlot ImGui::ImGui 
        PRIVATE ImPlot::Sources ImGui::Sources ImGui::Examples ImGui::SourcesMiscCpp
        ${grpc_required_libs})
      install_tool(${_examples})
    elseif(${_examples} STREQUAL "SmartPeakServer")
      add_executable(${_examples} source/${_examples})
      target_link_libraries(${_examples} PUBLIC ${SmartPeak_LIBRARIES} OpenMS PRIVATE ${grpc_required_libs})
      target_include_directories(${_examples} SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/source/service/)
    else()
      add_executable(${_examples} source/${_examples})
      target_link_libraries(${_examples} PUBLIC ${SmartPeak_LIBRARIES} OpenMS)
    endif()

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