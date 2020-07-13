cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project("SmartPeak")

#------------------------------------------------------------------------------
# naming conventions:
#
# prefix a variable with 'CF_' if it is used to configure a file!
# e.g., CF_LibSmartPeakExport
set(CF_SMARTPEAK_PACKAGE_VERSION "${SMARTPEAK_PACKAGE_VERSION_MAJOR}.${SMARTPEAK_PACKAGE_VERSION_MINOR}.${SMARTPEAK_PACKAGE_VERSION_PATCH}" CACHE INTERNAL "SmartPeak VERSION" FORCE)

#------------------------------------------------------------------------------
# En/disable assertions
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set(CF_SMARTPEAK_ASSERTIONS 1)
else()
	set(CF_SMARTPEAK_ASSERTIONS 0)
endif()
set(CF_SMARTPEAK_ASSERTIONS ${CF_SMARTPEAK_ASSERTIONS} CACHE INTERNAL "Enables debug messages (precondition and postconditions are enabled, a bit slower) - this is NOT changing any compiler flags!" FORCE)

#------------------------------------------------------------------------------
# external libs (contrib or system)
#------------------------------------------------------------------------------
include(${PROJECT_SOURCE_DIR}/cmake_findExternalLibs.cmake)

#------------------------------------------------------------------------------
# At this point make a summary of where data and doc will be located:
message(STATUS "Info: CF_SMARTPEAK_DATA_PATH: ${CF_SMARTPEAK_DATA_PATH}")
message(STATUS "Info: CF_SMARTPEAK_DOC_PATH: ${CF_SMARTPEAK_DOC_PATH}")

#------------------------------------------------------------------------------
# configure config.h
#------------------------------------------------------------------------------
include(${PROJECT_SOURCE_DIR}/configh.cmake)

#------------------------------------------------------------------------------
# big include file for headers and cpp files, that fills the SmartPeak_sources variable
include (${PROJECT_SOURCE_DIR}/includes.cmake)

#------------------------------------------------------------------------------
# all the dependency libraries are linked into libSmartPeak.so
set(SMARTPEAK_DEP_LIBRARIES ${OpenMS_LIBRARIES} ${SDL2_LIBRARIES}
                           ${Boost_LIBRARIES})

# xerces requires linking against CoreFoundation&CoreServices
if(APPLE)
  find_library(CoreFoundation_LIBRARY CoreFoundation )
  find_library(CoreServices_LIBRARY CoreServices )
  set(SMARTPEAK_DEP_LIBRARIES ${SMARTPEAK_DEP_LIBRARIES}
                           ${CoreFoundation_LIBRARY}
                           ${CoreServices_LIBRARY})
endif()

if (TBB_FOUND)
 list(APPEND SMARTPEAK_DEP_LIBRARIES ${TBB_LIBRARIES})
endif()
if (MSVC)
	list(APPEND SMARTPEAK_DEP_LIBRARIES opengl32.lib)
endif()

smartpeak_add_library(TARGET_NAME  SmartPeak
                   SOURCE_FILES  ${SmartPeak_sources}
                   HEADER_FILES  ${SmartPeak_sources_h}
                                 ${SmartPeak_configured_headers}
                   INTERNAL_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include
                                     ${CMAKE_CURRENT_BINARY_DIR}/include
                   EXTERNAL_INCLUDES ${OpenMS_INCLUDE_DIRECTORIES}
                                     ${SDL2_INCLUDE_DIRS}
                                     ${ImGui_INCLUDE_DIR}
                                     ${IMPLOT_INCLUDE_DIR}
                                     ${EIGEN3_INCLUDE_DIR}
                                     ${PLOG_INCLUDE_DIR}
                   LINK_LIBRARIES ${APPLE_EXTRA_LIBS}
                                  ${SMARTPEAK_DEP_LIBRARIES}
                   DLL_EXPORT_PATH "SmartPeak/")

#------------------------------------------------------------------------------
# since the share basically belongs to SmartPeak core we control its installation
# here
# Note: that slash(/) is important here, otherwise the whole directory
#       (not its content) will be copied!
install_directory(${SMARTPEAK_HOST_DIRECTORY}/share/SmartPeak/ ${INSTALL_SHARE_DIR} share)

#------------------------------------------------------------------------------
# register relevant paths for the doxygen doc generation
smartpeak_doc_path("${PROJECT_SOURCE_DIR}/include")

#-------------------------
# doxygen
set(SMARTPEAK_PUBLIC_HEADER_DIR ${PROJECT_SOURCE_DIR}/include/SmartPeak)
get_target_property(${SMARTPEAK_PUBLIC_HEADER_DIR} SmartPeak INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE SMARTPEAK_PUBLIC_HEADERS LIST_DIRECTORIES true ${SMARTPEAK_PUBLIC_HEADER_DIR}/*.h)


set(DOXYGEN_INPUT_DIR   ${PROJECT_SOURCE_DIR})
set(DOXYGEN_OUTPUT_DIR  ${CMAKE_CURRENT_BINARY_DIR}/docs)
set(DOXYGEN_INDEX_FILE  ${DOXYGEN_OUTPUT_DIR}/html/index.html )

set(DOXYFILE_IN   ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})


add_custom_command( OUTPUT            ${DOXYGEN_INDEX_FILE}
                    DEPENDS           ${SMARTPEAK_PUBLIC_HEADERS}
                    COMMAND           ${DOXYGEN_EXECUTABLE}         ${DOXYFILE_OUT}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    MAIN_DEPENDENCY   ${DOXYFILE_IN}                ${DOXYFILE_OUT}
                    COMMENT           "Generating docs for SmartPeak lib")


add_custom_target(libdoc-doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})


#sphinx
find_package(Sphinx REQUIRED)

set(SPHINX_SOURCE     ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD      ${CMAKE_CURRENT_BINARY_DIR}/docs/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)


add_custom_command( OUTPUT  ${SPHINX_INDEX_FILE} 
                    COMMAND ${SPHINX_EXECUTABLE} -b html
                    -Dbreathe_projects.SmartPeak=${DOXYGEN_OUTPUT_DIR}/xml
                    ${SPHINX_SOURCE}  ${SPHINX_BUILD}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    DEPENDS           ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
                                      ${DOXYGEN_INDEX_FILE}
                    MAIN_DEPENDENCY   ${SPHINX_SOURCE}/conf.py
                    COMMENT           "Generating sphinx docs for SmartPeak lib")

add_custom_target(libdoc-sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})

get_filename_component(SMARTPEAK_DOCS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../docs ABSOLUTE)

install(DIRECTORY ${SPHINX_BUILD}/ DESTINATION ${SMARTPEAK_DOCS_DIR})




