include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)
set (SQLITE_VERSION version-3.36.0)
message (STATUS "Adding SQLite ${SQLITE_VERSION} as an external project.")

ExternalProject_Add(sqlite
  URL https://www.sqlite.org/2021/sqlite-amalgamation-3360000.zip
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)