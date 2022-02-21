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
# $Maintainer: Bertrand Boudaud, Douglas McCloskey, Ahmed Khalil $
# $Authors: Bertrand Boudaud $
# --------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project("SmartPeakWidgets")

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
set(SMARTPEAK_DEP_LIBRARIES OpenMS OpenSSL::SSL OpenSSL::Crypto)
include_directories(${OPENSSL_INCLUDE_DIR})

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

smartpeak_add_library(TARGET_NAME  SmartPeakWidgets
                   SOURCE_FILES  ${SmartPeakWidgets_sources}
                   HEADER_FILES  ${SmartPeakWidgets_sources_h}
                                 ${SmartPeak_configured_headers}
                   INTERNAL_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include
                                     ${CMAKE_CURRENT_BINARY_DIR}/include
                   EXTERNAL_INCLUDES ${SmartPeak_INCLUDE_DIRECTORIES}
                                     ${ImGui_INCLUDE_DIR}
                                     ${IMPLOT_INCLUDE_DIR}
                                     ${EIGEN3_INCLUDE_DIR}
                                     ${OPENSSL_INCLUDE_DIR}
                                     ${PLOG_INCLUDE_DIR}
                                     ${PORTABLEFILEDIALOGS_INCLUDE_DIR}
                   LINK_LIBRARIES ${APPLE_EXTRA_LIBS}
                                  ${SMARTPEAK_DEP_LIBRARIES}
                   DLL_EXPORT_PATH "SmartPeak/")

#------------------------------------------------------------------------------
# since the share basically belongs to SmartPeak core we control its installation
# here
# Note: that slash(/) is important here, otherwise the whole directory
#       (not its content) will be copied!
install_directory(${SMARTPEAK_HOST_DIRECTORY}/share/SmartPeak/ ${INSTALL_SHARE_DIR} share)
