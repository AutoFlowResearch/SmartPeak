## Windows installer

## With VS2019 the architecture HAS TO BE specified with the "–A" option or CMAKE_GENERATOR_PLATFORM var.
## Therefore the legacy way of adding a suffix to the Generator is not valid anymore.
## Read value of CMAKE_VS_PLATFORM_NAME instead
if (CMAKE_VS_PLATFORM_NAME MATCHES ".*Win32.*" OR CMAKE_GENERATOR MATCHES ".*Win32.*")
  set(PLATFORM "32")
  set(ARCH "x86")
else()
  set(PLATFORM "64")
  set(ARCH "x64")
endif()
set(VC_REDIST_EXE "vcredist_${ARCH}.exe")


## Find redistributable to be installed by NSIS
if (NOT VC_REDIST_PATH)
	string(REGEX REPLACE ".*Visual Studio ([1-9][1-9]) .*" "\\1" SMARTPEAK_MSVC_VERSION_STRING "${CMAKE_GENERATOR}")
	if("${SMARTPEAK_MSVC_VERSION_STRING}" GREATER "14")  
	  ## We have to glob recurse in the parent folder because there is a version number in the end.
	  ## Unfortunately in my case the default version (latest) does not include the redist?!
	  ## TODO Not sure if this environment variable always exists. In the VS command line it should! Fallback vswhere or VCINSTALLDIR/Redist/MSVC?
	  get_filename_component(VC_ROOT_PATH "$ENV{VCToolsRedistDir}.." ABSOLUTE)
	  file(GLOB_RECURSE VC_REDIST_ABS_PATH "${VC_ROOT_PATH}/${VC_REDIST_EXE}")
	  ## TODO pick the latest of the found redists
	  get_filename_component(VC_REDIST_PATH "${VC_REDIST_ABS_PATH}" DIRECTORY)
	elseif(SMARTPEAK_MSVC_VERSION_STRING GREATER "10")
	  set(VC_REDIST_PATH "$ENV{VCINSTALLDIR}redist/1033")  
	else()
	  message(FATAL_ERROR "Variable VC_REDIST_PATH missing."
	  "Before Visual Studio 2012 you have to provide the path"
	  "to the redistributable package of the VS you are using on your own.")
	endif()
endif()

##TODO try following instead once CMake generates NSIS commands for us. Installs dll instead of redist though. Thirdparties?
# ########################################################### System runtime libraries
#set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
#include(InstallRequiredSystemLibraries)
#install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
#        DESTINATION SmartPeak-${CPACK_PACKAGE_VERSION}/${PACKAGE_LIB_DIR}/
#        COMPONENT library)

set(CPACK_GENERATOR NSIS)
## Remove the next three lines if you use the NSIS autogeneration feature at some point!
## For now it makes sure everything is merged into the usual folders bin/share/include
set(CPACK_COMPONENT_ALL_IN_ONE 1)
set(CPACK_COMPONENTS_ALL_GROUPS_IN_ONE_PACKAGE 1)
set(CPACK_MONOLITHIC_INSTALL 1)

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${SMARTPEAK_PACKAGE_VERSION_MAJOR}.${SMARTPEAK_PACKAGE_VERSION_MINOR}.${SMARTPEAK_PACKAGE_VERSION_PATCH}-Win${PLATFORM}")
#set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/cmake/Windows/SmartPeak.ico")

## Create own target because you cannot "depend" on the internal target 'package'
add_custom_target(dist
  COMMAND cpack -G ${CPACK_GENERATOR}
  COMMENT "Building ${CPACK_GENERATOR} package"
)

## TODO maybe find signtool and maybe check existence of ID in the beginning.
## ID needs to be installed beforehand. Rightclick a p12 file that has a cert for codesigning.
if (DEFINED SIGNING_IDENTITY AND NOT "${SIGNING_IDENTITY}" STREQUAL "") 
	add_custom_target(signed_dist
	  COMMAND signtool sign /v /n "${SIGNING_IDENTITY}" /t http://timestamp.digicert.com ${CPACK_PACKAGE_FILE_NAME}.exe
	  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
	  COMMENT "Signing ${CPACK_PACKAGE_FILE_NAME}.exe with '${SIGNING_IDENTITY}'"
	  DEPENDS dist
	)
endif()

## For now we fully rely only on our NSIS template. Later we could use
## the following to let CMake generate snippets for the NSIS script
## Plus an additional entry in the nsis template (see CPack-NSIS docu)

# set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/cmake/Windows/SmartPeak.ico")
# set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/cmake/Windows/SmartPeak.ico")
# set(CPACK_NSIS_HELP_LINK "https://www.SmartPeak.com/getting-started")
# set(CPACK_NSIS_URL_INFO_ABOUT "https://www.SmartPeak.com")
# set(CPACK_NSIS_CONTACT "smartpeak-general@lists.sourceforge.net")
# set(CPACK_NSIS_MENU_LINKS
#     "https://www.SmartPeak.com" "SmartPeak Web Site")



