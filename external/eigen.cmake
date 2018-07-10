include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)

set (EIGEN_VERSION 3.3.4)
message (STATUS "Adding Eigen ${EIGEN_VERSION} as an external project.")

#if (WIN32)
#    set (EIGEN_INSTALL_CMD "move Eigen unsupported <INSTALL_DIR>/")
#elseif (UNIX)
#    set (EIGEN_INSTALL_CMD "mv Eigen unsupported <INSTALL_DIR>/")
#endif ()

ExternalProject_Add(eigen
  GIT_REPOSITORY "https://github.com/eigenteam/eigen-git-mirror.git"
  #GIT_TAG ${EIGEN_VERSION} # Need the dev branch to compile use MSVC
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE 1
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  #INSTALL_COMMAND ${EIGEN_INSTALL_CMD}
  #INSTALL_DIR include
)