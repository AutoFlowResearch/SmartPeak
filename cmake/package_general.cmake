###
# Packaging Script for CPack
#
# Components to package are share, library, applications, and docs
#
# The specific components are installed in the various CMake scripts during compile time
# using the helper macros found in export_macros.cmake and install_macros.cmake.
#
# CPack generator specific scripts are needed to ensure the application runs on each OS :(

# --------------------------------------------------------------------------
# general definitions used for building SmartPeak packages
set(CPACK_PACKAGE_NAME "SmartPeak")
set(CPACK_PACKAGE_VENDOR "SmartPeak.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SmartPeak - metabolomics, lipidomics, proteomics, and fluxomics data processing")
set(CPACK_PACKAGE_VERSION "${SMARTPEAK_PACKAGE_VERSION_MAJOR}.${SMARTPEAK_PACKAGE_VERSION_MINOR}.${SMARTPEAK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${SMARTPEAK_PACKAGE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${SMARTPEAK_PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${SMARTPEAK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SmartPeak-${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/cmake/SmartPeakPackageDescriptionFile.cmake)
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/License.txt)
set(CPACK_RESOURCE_FILE_WELCOME ${PROJECT_SOURCE_DIR}/cmake/SmartPeakPackageResourceWelcomeFile.txt)
set(CPACK_RESOURCE_FILE_README ${PROJECT_SOURCE_DIR}/cmake/SmartPeakPackageResourceReadme.txt)

# --------------------------------------------------------------------------
# general components and groupings for SmartPeak installation
set(CPACK_COMPONENTS_ALL share library applications doc)
set(CPACK_COMPONENT_SHARE_DISPLAY_NAME "SmartPeak shared files")
set(CPACK_COMPONENT_LIBRARY_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "SmartPeak binaries")
set(CPACK_COMPONENT_DOC_DISPLAY_NAME "Documentation")
set(CPACK_COMPONENT_SHARE_DESCRIPTION "SmartPeak shared files")
set(CPACK_COMPONENT_LIBRARY_DESCRIPTION "SmartPeak libraries for building applications against")
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION "SmartPeak executables to run the GUI or scripts")
set(CPACK_COMPONENT_DOC_DESCRIPTION "Class documentation with tutorials")
set(CPACK_COMPONENT_SHARE_GROUP "Development")
set(CPACK_COMPONENT_LIBRARY_GROUP "Runtime")
set(CPACK_COMPONENT_APPLICATIONS_GROUP "Runtime")
set(CPACK_COMPONENT_DOC_GROUP "Runtime")
set(CPACK_ALL_INSTALL_TYPES Full Developer)
set(CPACK_COMPONENT_SHARE_INSTALL_TYPES Developer Full)
set(CPACK_COMPONENT_LIBRARY_INSTALL_TYPES Full)
set(CPACK_COMPONENT_APPLICATIONS_INSTALL_TYPES Full)
set(CPACK_COMPONENT_DOC_INSTALL_TYPES Full)
  
# --------------------------------------------------------------------------
# runtime code to ensure the .exe files are "fixed up" by copying all needed runtime components
# into the same directory.
# NOTES:
# 1. All .exe's in the same directory can be accumulated using GET_BUNDLE_ALL_EXECUTABLES or manually specified
# 2. fixup_bundle does not copy the dependencies to the installation target; hence we manually copy the bundle to the target and fixup_bundle in place
# 3. fixup_bundle does not handle OpenMS shared files.
# 4. certain directories must be rediscovered again (i.e., SDL2 and OpenMS)
find_package(SDL2 REQUIRED)
if (SDL2_FOUND)
  # Add the directory for later calls to CPACK
  string(FIND ${SDL2_LIBRARY} ${VC_LIB_PATH_SUFFIX} SDL2_library_suffix_pos)
  string(SUBSTRING ${SDL2_LIBRARY} 0 ${SDL2_library_suffix_pos} SDL2_LIBRARY_DIR)
  set(SDL2_LIBRARIES_DIR "${SDL2_LIBRARY_DIR}${VC_LIB_PATH_SUFFIX}")
  MESSAGE(STATUS "SDL2_LIBRARIES_DIR ${SDL2_LIBRARIES_DIR}")
endif (SDL2_FOUND)
find_package(OpenMS REQUIRED)
if (OpenMS_FOUND)
  ## Add the directory for later calls to CPACK
  set(OpenMS_LIBRARY_DIR ${OpenMS_DIR}/lib/Release)
  MESSAGE(STATUS "OpenMS_LIBRARY_DIR ${OpenMS_LIBRARY_DIR}")
  string(FIND ${OpenMS_DIR} \"/\" OpenMS_DIR_suffix_pos REVERSE)
  string(SUBSTRING ${OpenMS_DIR} 0 ${OpenMS_DIR_suffix_pos} OpenMS_ROOT)
  set(OpenMS_SHARE_DIR ${OpenMS_ROOT}/share/OpenMS)
  MESSAGE(STATUS "OpenMS_SHARE_DIR ${OpenMS_SHARE_DIR}")
endif (OpenMS_FOUND)
install(CODE "
 include(InstallRequiredSystemLibraries)
 include(BundleUtilities)
 # GET_BUNDLE_ALL_EXECUTABLES(${CMAKE_RUNTIME_OUTPUT_DIRECTORY} EXECS)
 SET(EXECS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/GUI.exe)
 SET(DEST \"\${CMAKE_INSTALL_PREFIX}/bin/GUI.exe\")
 execute_process(COMMAND ${CMAKE_COMMAND} -E copy \"\${EXECS}\" \"\${DEST}\")
 # SET(DIRS ${Qt5Core_DIR} ${Qt5Network_DIR} ${Qt5_DIR} C:/Users/domccl/GitHub/OpenMS/openms-build/bin/Release ${BOOST_LIBRARYDIR} C:/Users/domccl/GitHub/SDL/lib/x64)
 # fixup_bundle(\"\${DEST}\" \"\" \"\${DIRS}\")
 # execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory C:/Users/domccl/GitHub/OpenMS/share/OpenMS \"\${CMAKE_INSTALL_PREFIX}/bin/OpenMS\")
 SET(DIRS ${Qt5Core_DIR} ${Qt5Network_DIR} ${Qt5_DIR} ${OpenMS_LIBRARY_DIR} ${BOOST_LIBRARYDIR} ${SDL2_LIBRARIES_DIR})
 fixup_bundle(\"\${DEST}\" \"\" \"\${DIRS}\")
 execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory \"\${OpenMS_SHARE_DIR}\" \"\${CMAKE_INSTALL_PREFIX}/bin/OpenMS\")
 " 
 COMPONENT applications)

