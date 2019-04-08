include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)
set (PLOG_VERSION 5f69a631086bf038e2c0feee4319a485c34fa863)
message (STATUS "Adding plog ${PLOG_VERSION} as an external project.")

ExternalProject_Add(plog
  GIT_REPOSITORY https://github.com/SergiusTheBest/plog.git
  GIT_TAG ${PLOG_VERSION}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)