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
