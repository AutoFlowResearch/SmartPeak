cpack_add_install_type(recommended DISPLAY_NAME "Recommended")
cpack_add_install_type(full DISPLAY_NAME "Full")
cpack_add_install_type(minimal DISPLAY_NAME "Minimal")

cpack_add_component(share
                DISPLAY_NAME "SmartPeak shared files"
                DESCRIPTION "SmartPeak shared files"
                INSTALL_TYPES recommended full minimal
                )
cpack_add_component(library
                DISPLAY_NAME "Libraries"
                DESCRIPTION "Libraries"
                INSTALL_TYPES recommended full minimal
                )
cpack_add_component(applications
                DISPLAY_NAME "SmartPeak binaries"
                DESCRIPTION "SmartPeak binaries."
                INSTALL_TYPES recommended full minimal
                )
cpack_add_component(doc
                DISPLAY_NAME "Documentation"
                DESCRIPTION "Class documentation with tutorials."
                INSTALL_TYPES recommended full
                )
install(CODE "
 include(InstallRequiredSystemLibraries)
 include(BundleUtilities)
 # GET_BUNDLE_ALL_EXECUTABLES(${CMAKE_RUNTIME_OUTPUT_DIRECTORY} EXECS)
 # SET(EXECS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/GUI.exe)
 SET(DEST \"\${CMAKE_INSTALL_PREFIX}/bin/GUI.exe\")
 SET(DIRS ${Qt5Core_DIR} ${Qt5Network_DIR} ${Qt5_DIR} C:/Users/domccl/GitHub/OpenMS/openms-build/bin/Release ${BOOST_LIBRARYDIR} C:/Users/domccl/GitHub/SDL/lib/x64)
 fixup_bundle(\"\${DEST}\" \"\" \"\${DIRS}\")
 # copy_and_fixup_bundle(\"\${EXECS}\" \"\${DEST}\" \"\" \"\${DIRS}\")
 " 
 COMPONENT Runtime)