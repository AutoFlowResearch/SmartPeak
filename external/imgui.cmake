include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)
#set (IMGUI_VERSION 6a0d0dab5a9f0b9518a2bc9bb456a69895ae0962) # tag: v1.72b, 2019/07/31
set (IMGUI_VERSION v1.81) # tables
message (STATUS "Adding imgui ${IMGUI_VERSION} as an external project.")

ExternalProject_Add(imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG ${IMGUI_VERSION}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)