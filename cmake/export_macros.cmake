# --------------------------------------------------------------------------
#                   SmartPeak -- Open-Source Mass Spectrometry
# --------------------------------------------------------------------------
# Copyright The SmartPeak Team -- Eberhard Karls University Tuebingen,
# ETH Zurich, and Freie Universitaet Berlin 2002-2017.
#
# This software is released under a three-clause BSD license:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of any author or any participating institution
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
# For a full list of authors, refer to the file AUTHORS.
# --------------------------------------------------------------------------
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
# $Maintainer: Stephan Aiche $
# $Authors: Stephan Aiche $
# --------------------------------------------------------------------------

# a collection of wrapper for export functions that allows easier usage
# througout the SmartPeak build system
set(_SMARTPEAK_EXPORT_FILE "SmartPeakTargets.cmake")

# clear list before we refill it
set(_SMARTPEAK_EXPORT_TARGETS "" CACHE INTERNAL "List of targets that will be exported.")

macro(smartpeak_register_export_target target_name)
  set(_SMARTPEAK_EXPORT_TARGETS ${_SMARTPEAK_EXPORT_TARGETS} ${target_name}
    CACHE INTERNAL "List of targets that will be exported.")
endmacro()

macro(smartpeak_export_targets )
  set(_EXPORT_INCLUDE_BLOCK "")

  foreach(_target ${_SMARTPEAK_EXPORT_TARGETS})
    # check if we have a corresponding include_dir variable
    if(NOT DEFINED ${_target}_INCLUDE_DIRECTORIES)
      message(FATAL_ERROR "Please provide the matching include directory variable ${_target}_INCLUDE_DIRECTORIES for export target ${_target}")
    endif()

    # extend include block
    set(_EXPORT_INCLUDE_BLOCK "set(${_target}_INCLUDE_DIRECTORIES \"${${_target}_INCLUDE_DIRECTORIES}\")\n\n${_EXPORT_INCLUDE_BLOCK}")
  endforeach()

  # configure SmartPeakConfig.cmake
  configure_file(
    "${SMARTPEAK_HOST_DIRECTORY}/cmake/SmartPeakConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/SmartPeakConfig.cmake"
    @ONLY
  )

  # configure SmartPeakConfig.cmake
  configure_file(
    "${SMARTPEAK_HOST_DIRECTORY}/cmake/SmartPeakConfigVersion.cmake.in"
    "${PROJECT_BINARY_DIR}/SmartPeakConfigVersion.cmake"
    @ONLY
  )

  # create corresponding target file
  export(TARGETS ${_SMARTPEAK_EXPORT_TARGETS}
         FILE ${SMARTPEAK_HOST_BINARY_DIRECTORY}/${_SMARTPEAK_EXPORT_FILE})

  # --------------------------------------------------------------------------
  # export for install; clear variable before refilling
  set(_EXPORT_INCLUDE_BLOCK "")
  foreach(_target ${_SMARTPEAK_EXPORT_TARGETS})
    # check if we have a corresponding include_dir variable
    if(NOT DEFINED ${_target}_INCLUDE_DIRECTORIES)
      message(FATAL_ERROR "Please provide the matching include directory variable ${_target}_INCLUDE_DIRECTORIES for export target ${_target}")
    endif()

    # find all includes that will not be installed with SmartPeak
    set(_NON_INSTALLABLE_INCLUDES "")
    foreach(_incl_path ${${_target}_INCLUDE_DIRECTORIES})
      if (NOT "${_incl_path}" MATCHES "^${SMARTPEAK_HOST_DIRECTORY}.*" AND NOT "${_incl_path}" MATCHES "^${SMARTPEAK_HOST_BINARY_DIRECTORY}.*")
        set(_NON_INSTALLABLE_INCLUDES ${_NON_INSTALLABLE_INCLUDES}
                                      ${_incl_path})
      endif()
    endforeach()

    # append install include dir
    set(_NON_INSTALLABLE_INCLUDES ${_NON_INSTALLABLE_INCLUDES}
                                  ${INSTALL_INCLUDE_DIR})
    set(_EXPORT_INCLUDE_BLOCK "set(${_target}_INCLUDE_DIRECTORIES \"${_NON_INSTALLABLE_INCLUDES}\")\n\n${_EXPORT_INCLUDE_BLOCK}")
  endforeach()

  # configure SmartPeakConfig.cmake
  configure_file(
    "${SMARTPEAK_HOST_DIRECTORY}/cmake/SmartPeakConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/install/SmartPeakConfig.cmake"
    @ONLY
  )

  # install the generated config file
  install_file(${PROJECT_BINARY_DIR}/install/SmartPeakConfig.cmake
               ${INSTALL_SHARE_DIR}/cmake
               share)

  # .. and ConfigVersion.cmake
  install_file(${PROJECT_BINARY_DIR}/SmartPeakConfigVersion.cmake
               ${INSTALL_SHARE_DIR}/cmake
               share)

  # register the package
  export(PACKAGE SmartPeak)
endmacro()
