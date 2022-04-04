include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)
set (PORTABLE_FILE_DIALOG_VERSION fix/macos_save_as)
message (STATUS "Adding portable-file-dialogs ${PORTABLE_FILE_DIALOG_VERSION} as an external project.")

ExternalProject_Add(portablefiledialogs
  GIT_REPOSITORY https://github.com/bertrandboudaud/portable-file-dialogs.git
  GIT_TAG ${PORTABLE_FILE_DIALOG_VERSION}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)