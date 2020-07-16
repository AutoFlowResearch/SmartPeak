
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "SmartPeak developers <smartpeak-general@lists.sourceforge.net>")
if (SMARTPEAK_64BIT_ARCHITECTURE)
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${SMARTPEAK_PACKAGE_VERSION_FULLSTRING}-Debian-Linux-x86_64")
else()
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${SMARTPEAK_PACKAGE_VERSION_FULLSTRING}-Debian-Linux-x86")
endif()
set(CPACK_GENERATOR "DEB")

## CPack issues when building the package.
## https://bugs.launchpad.net/ubuntu/+source/cmake/+bug/972419
## https://ubuntuforums.org/showthread.php?t=2316865
## Workaround after packaging: https://cmake.org/pipermail/cmake/2012-May/050483.html
## Following needs CMake 3.7+. Just install from cmake.org
set(CPACK_DEBIAN_ARCHIVE_TYPE "gnutar")

## We usually do not want to ship things like stdlib or glibc. Could mess up a system slighlty, when installed system wide
#include(InstallRequiredSystemLibraries)

## Try autogeneration of dependencies:
## This may result in non-standard package names in the dependencies (e.g. when using Qt from a Thirdparty repo)
## It also will add system dependencies like a minimum glibc or gomp version (not necessarily bad)
##set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

## Debug for now. Not much output.
set(CPACK_DEBIAN_PACKAGE_DEBUG ON)

SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
SET(CPACK_DEBIAN_PACKAGE_SECTION "science")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "package for metabolomics, lipidomics, and fluxomics data processing")
SET(CPACK_PACKAGE_DESCRIPTION "
 SmartPeak automates metabolomics, lipidomics, proteomics, and fluxomics data processing"
 )

## Create own target because you cannot "depend" on the internal target 'package'
add_custom_target(dist
  COMMAND cpack -G ${CPACK_GENERATOR}
  COMMENT "Building ${CPACK_GENERATOR} package"
)