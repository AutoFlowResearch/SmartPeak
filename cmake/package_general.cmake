# --------------------------------------------------------------------------
# Packaging Script for CPack
#
# Components to package are share, library, applications, and docs
#
# The specific components are installed in the various CMake scripts during compile time
# using the helper macros found in export_macros.cmake and install_macros.cmake.
#
# CPack generator specific scripts are needed to ensure the application runs on each OS :(
# --------------------------------------------------------------------------

# --------------------------------------------------------------------------
# general definitions used for building SmartPeak packages
set(CPACK_PACKAGE_NAME                  "SmartPeak")
set(CPACK_PACKAGE_VENDOR                "SmartPeak.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   "SmartPeak - metabolomics, lipidomics, proteomics, and fluxomics data processing")
set(CPACK_PACKAGE_VERSION               "${SMARTPEAK_PACKAGE_VERSION_MAJOR}.${SMARTPEAK_PACKAGE_VERSION_MINOR}.${SMARTPEAK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR         "${SMARTPEAK_PACKAGE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR         "${SMARTPEAK_PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH         "${SMARTPEAK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY     "SmartPeak-${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_FILE      ${PROJECT_SOURCE_DIR}/cmake/SmartPeakPackageDescriptionFile.cmake)
# set(CPACK_RESOURCE_FILE_LICENSE     ${PROJECT_SOURCE_DIR}/License.txt)
set(CPACK_RESOURCE_FILE_WELCOME         ${PROJECT_SOURCE_DIR}/cmake/SmartPeakPackageResourceWelcomeFile.txt)
set(CPACK_RESOURCE_FILE_README          ${PROJECT_SOURCE_DIR}/cmake/SmartPeakPackageResourceReadme.txt)



if (APPLE)
  set(CPACK_GENERATOR           "DragNDrop")
  set(BUNDLE_OUTPUT_PATH        "${CMAKE_BINARY_DIR}/bin/GUI.app")
  set(CPACK_MONOLITHIC_INSTALL  ON)
  set(CPACK_BUNDLE_NAME         "SmartPeak")
  set(CPACK_BUNDLE_PLIST        ${PROJECT_SOURCE_DIR}/cmake/MacOSX/Info.plist)
  set(CPACK_BUNDLE_ICON         ${PROJECT_SOURCE_DIR}/cmake/MacOSX/sp.icns)

  # set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0) 
  # set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY 1)
  # # use, i.e. don't skip the full RPATH for the build tree
  # set(CMAKE_SKIP_BUILD_RPATH FALSE)

  # # when building, don't use the install RPATH already
  # # (but later on when installing)
  # set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

  # # the RPATH to be used when installing
  # set(CMAKE_INSTALL_RPATH "")

  # # don't add the automatically determined parts of the RPATH
  # # which point to directories outside the build tree to the install RPATH
  # set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  # set(CPACK_COMMAND_CODESIGN "codesign -o runtime --timestamp")
  # SET(CPACK_BUNDLE_APPLE_CERT_APP "Apple Development: Ahmed Khalil (Ahmed Khalil)")
  # # SET(CPACK_APPLE_BUNDLE_ID "com.smartpeak")
endif()

# --------------------------------------------------------------------------
# general components and groupings for SmartPeak installation
set(CPACK_COMPONENTS_ALL                        share library applications doc)
set(CPACK_COMPONENT_SHARE_DISPLAY_NAME          "SmartPeak shared files")
set(CPACK_COMPONENT_LIBRARY_DISPLAY_NAME        "Libraries")
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME   "SmartPeak binaries")
set(CPACK_COMPONENT_DOC_DISPLAY_NAME            "Documentation")
set(CPACK_COMPONENT_SHARE_DESCRIPTION           "SmartPeak shared files")
set(CPACK_COMPONENT_LIBRARY_DESCRIPTION         "SmartPeak libraries for building applications against")
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION    "SmartPeak executables to run the GUI or scripts")
set(CPACK_COMPONENT_DOC_DESCRIPTION             "Class documentation with tutorials")
set(CPACK_COMPONENT_SHARE_GROUP                 "Development")
set(CPACK_COMPONENT_LIBRARY_GROUP               "Runtime")
set(CPACK_COMPONENT_APPLICATIONS_GROUP          "Runtime")
set(CPACK_COMPONENT_DOC_GROUP                   "Runtime")
set(CPACK_ALL_INSTALL_TYPES                     Full Developer)
set(CPACK_COMPONENT_SHARE_INSTALL_TYPES         Developer Full)
set(CPACK_COMPONENT_LIBRARY_INSTALL_TYPES       Full)
set(CPACK_COMPONENT_APPLICATIONS_INSTALL_TYPES  Full)
set(CPACK_COMPONENT_DOC_INSTALL_TYPES           Full)
  
# --------------------------------------------------------------------------
# runtime code to ensure the .exe files are "fixed up" by copying all needed runtime components
# into the same directory.
# NOTES:
# 1. All .exe's in the same directory can be accumulated using GET_BUNDLE_ALL_EXECUTABLES or manually specified (as is the case currently)
# 2. fixup_bundle does not copy the dependencies to the installation target; hence we manually copy the bundle to the target and fixup_bundle in place
# 3. fixup_bundle does not handle OpenMS shared files nor environmental variables to the shared files.
# 4. certain directories must be rediscovered again (i.e., SDL2 and OpenMS)
find_package(SDL2 REQUIRED)
if (SDL2_FOUND)
  if (WIN32)
    # Add the directory for later calls to CPACK
    string(FIND ${SDL2_LIBRARY} ${VC_LIB_PATH_SUFFIX} SDL2_library_suffix_pos)
    string(SUBSTRING ${SDL2_LIBRARY} 0 ${SDL2_library_suffix_pos} SDL2_LIBRARY_DIR)
    set(SDL2_LIBRARIES_DIR "${SDL2_LIBRARY_DIR}${VC_LIB_PATH_SUFFIX}")
  elseif(APPLE)
    set(SDL2_LIBRARIES_DIR "")
	  string(REGEX REPLACE ";.*$" "" SDL2_LIB_DIR "${SDL2_LIBRARY}")
	  if(EXISTS "${SDL2_LIB_DIR}/Versions/A/Frameworks")
      set(SDL2_LIBRARIES_DIR "${SDL2_LIB_DIR}/Versions/A")
      set(HIDAPI_DIR "${SDL2_LIB_DIR}/Versions/A/Frameworks")
	  endif()
  endif()
endif (SDL2_FOUND)

find_package(OpenMS REQUIRED)
if (OpenMS_FOUND)
  ## Add the directory for later calls to CPACK
  set(OpenMS_LIBRARY_DIR ${OpenMS_DIR}/lib)
  string(FIND ${OpenMS_DIR} "/" OpenMS_DIR_suffix_pos REVERSE)
  string(SUBSTRING ${OpenMS_DIR} 0 ${OpenMS_DIR_suffix_pos} OpenMS_ROOT)
  set(OpenMS_SHARE_DIR ${OpenMS_ROOT}/share/OpenMS)
  # message(">>{OpenMS_ROOT}" ${OpenMS_ROOT})
  # message(">>{OpenMS_DIR}" ${OpenMS_DIR})
endif (OpenMS_FOUND)

if (WIN32)
  install(CODE "
  include(InstallRequiredSystemLibraries)
  include(BundleUtilities)
  # GET_BUNDLE_ALL_EXECUTABLES(${CMAKE_RUNTIME_OUTPUT_DIRECTORY} EXECS)
  SET(EXECS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/GUI.exe)
  SET(DEST \"\${CMAKE_INSTALL_PREFIX}/bin/GUI.exe\")
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy \"\${EXECS}\" \"\${DEST}\")
  SET(DIRS ${Qt5Core_DIR} ${Qt5Network_DIR} ${Qt5_DIR} ${BOOST_LIBRARYDIR} ${SDL2_LIBRARIES_DIR})
  fixup_bundle(\"\${DEST}\" \"\" \"\${DIRS}\")
  " 
  COMPONENT applications)
  install(CODE "
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${OpenMS_SHARE_DIR} \"\${CMAKE_INSTALL_PREFIX}/share/OpenMS\")
  " 
  COMPONENT share)

 elseif(APPLE)

  install(CODE "
  set(BU_CHMOD_BUNDLE_ITEMS ON)
  set(BU_COPY_FULL_FRAMEWORK_CONTENTS ON)
  include(InstallRequiredSystemLibraries)
  include(BundleUtilities)
 
  SET(DIRS ${Qt5Core_DIR} ${Qt5Network_DIR} ${Qt5_DIR} ${OpenMS_LIBRARY_DIR} ${BOOST_LIBRARYDIR}  ${SDL2_LIBRARIES_DIR} ${HIDAPI_DIR})
  fixup_bundle(\"${BUNDLE_OUTPUT_PATH}\" \"\" \"\${DIRS}\")
  verify_app(\"${BUNDLE_OUTPUT_PATH}\")
  " 
  COMPONENT applications)
  
  install(CODE "
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${OpenMS_SHARE_DIR} \"\${CMAKE_INSTALL_PREFIX}/share/OpenMS\")
  " 
  COMPONENT share)

 endif()

