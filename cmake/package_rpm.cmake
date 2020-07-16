set(CPACK_GENERATOR "RPM")


set(CPACK_RPM_PACKAGE_VENDOR "SmartPeak developers <open-ms-general@lists.sourceforge.net>")
if (SMARTPEAK_64BIT_ARCHITECTURE)
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${SMARTPEAK_PACKAGE_VERSION_FULLSTRING}-RedHat-Linux-x86_64")
else()
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${SMARTPEAK_PACKAGE_VERSION_FULLSTRING}-RedHat-Linux-x86")
endif()

## Debug for now.
set(CPACK_RPM_PACKAGE_DEBUG ON)

## TODO also install headers? make a dev package configuration?
set(CPACK_COMPONENTS_ALL applications doc library share ${THIRDPARTY_COMPONENT_GROUP})

SET(CPACK_RPM_PACKAGE_LICENSE "BSD clause 3")
#SET(CPACK_RPM_PACKAGE_AUTOREQ ON)
#SET(CPACK_RPM_PACKAGE_AUTOPROV ON)
SET(CPACK_RPM_PACKAGE_AUTOREQPROV ON)
SET(CPACK_RPM_COMPRESSION_TYPE xz)
SET(CPACK_RPM_INSTALL_WITH_EXEC ON)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "metabolomics, lipidomics, proteomics, and fluxomics data processing")
SET(CPACK_PACKAGE_DESCRIPTION "
 SmartPeak automates metabolomics, lipidomics, proteomics, and fluxomics data processing."
 )

## Create own target because you cannot "depend" on the internal target 'package'
add_custom_target(dist
  COMMAND cpack -G ${CPACK_GENERATOR}
  COMMENT "Building ${CPACK_GENERATOR} package"
)