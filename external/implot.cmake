include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)
set (DEPENDENCIES)
set (IMPLOT_VERSION v0.4)
message (STATUS "Adding ImPlot ${IMPLOT_VERSION} as an external project.")

ExternalProject_Add(implot
  GIT_REPOSITORY "https://github.com/epezent/implot.git"
  GIT_TAG ${IMPLOT_VERSION}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)