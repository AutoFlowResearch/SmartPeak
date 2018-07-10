
# --------------------------------------------------------------------------
# general definitions used for building SmartPeak packages
set(CPACK_PACKAGE_NAME "SmartPeak")
set(CPACK_PACKAGE_VENDOR "SmartPeak.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SmartPeak - A framework for mass spectrometry")
set(CPACK_PACKAGE_VERSION "${OPENMS_PACKAGE_VERSION_MAJOR}.${OPENMS_PACKAGE_VERSION_MINOR}.${OPENMS_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${OPENMS_PACKAGE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${OPENMS_PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${OPENMS_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SmartPeak-${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/cmake/SmartPeakPackageDescriptionFile.cmake)
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/License.txt)
set(CPACK_RESOURCE_FILE_WELCOME ${PROJECT_SOURCE_DIR}/cmake/SmartPeakPackageResourceWelcomeFile.txt)
set(CPACK_RESOURCE_FILE_README ${PROJECT_SOURCE_DIR}/cmake/SmartPeakPackageResourceReadme.txt)

########################################################### Fixing dynamic dependencies
# Done on Windows via copying external and internal dlls to the install/bin/ folder
# Done on Mac via fixup_bundle for the GUI apps (TOPPView, TOPPAS) and via fix_mac_dependencies for the TOPP tools
# which recursively gathers dylds, copies them to install/lib/ and sets the install_name of the binaries to @executable_path/../lib
# Not done on Linux. Either install systemwide (omit CMAKE_INSTALL_PREFIX or set it to /usr/) or install and add the
# install/lib/ folder to the LD_LIBRARY_PATH

#install(CODE "
#  include(BundleUtilities)
#  GET_BUNDLE_ALL_EXECUTABLES(\${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN_DIR} EXECS)
#  fixup_bundle(\"${EXECS}\" \"\" \"\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}\")
#  " COMPONENT applications)