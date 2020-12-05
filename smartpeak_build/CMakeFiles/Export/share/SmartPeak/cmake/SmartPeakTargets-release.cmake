#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SmartPeak" for configuration "Release"
set_property(TARGET SmartPeak APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SmartPeak PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/SmartPeak.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS SmartPeak )
list(APPEND _IMPORT_CHECK_FILES_FOR_SmartPeak "${_IMPORT_PREFIX}/bin/SmartPeak.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
