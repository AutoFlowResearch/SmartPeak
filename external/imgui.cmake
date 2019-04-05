include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)
set (IMGUI_VERSION ebe79bbed00a13fd4455f04131b63d49c28ebd5d) # tag: v1.69
message (STATUS "Adding imgui ${IMGUI_VERSION} as an external project.")

ExternalProject_Add(imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG ${IMGUI_VERSION}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)