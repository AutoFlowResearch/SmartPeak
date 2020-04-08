
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