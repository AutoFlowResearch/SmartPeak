include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)

set (IMPLOT_VERSION master)
message (STATUS "Adding ImPlot ${IMPLOT_VERSION} as an external project.")

ExternalProject_Add(implot
  GIT_REPOSITORY "https://https://github.com/epezent/implot.git"
  GIT_TAG ${IMPLOT_VERSION}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE 1
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  #INSTALL_COMMAND ${EIGEN_INSTALL_CMD}
  #INSTALL_DIR include
)