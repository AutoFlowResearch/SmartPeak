
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

# set(APPS ${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN_DIR}/GUI.exe)
# set(DIRS ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR})
# #set(DIRS ${SmartPeak_LIBRARIES} OpenMS ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ImGui::ImGui PRIVATE ImGui::Sources ImGui::Examples ImGui::SourcesMiscCpp)
# #set(LIBS \${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}/SmartPeak${CMAKE_STATIC_LIBRARY_SUFFIX})
# install(CODE "
        # include(BundleUtilities)
        # fixup_bundle(\"${APPS}\"   \"\"   \"${DIRS}\")"
    # COMPONENT Runtime)
	
# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# where CPackConfig.cmake is created by including CPack
# And then there's ways to customize this as well
#set(CPACK_BINARY_DRAGNDROP ON)

# install(CODE "
 # include(BundleUtilities)
 # GET_BUNDLE_ALL_EXECUTABLES(${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN_DIR} EXECS)
 # fixup_bundle(\${EXECS}\" \"\" \"\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}\")
 # " COMPONENT applications)