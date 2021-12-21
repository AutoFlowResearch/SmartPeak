include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)
set (IMGUI_VERSION 848d21b6b56d369fb7976bd88b5f4094ab9ea50a) # on Docking branch
message (STATUS "Adding imgui ${IMGUI_VERSION} as an external project.")

ExternalProject_Add(imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG ${IMGUI_VERSION}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  PATCH_COMMAND git apply ${CMAKE_CURRENT_LIST_DIR}/patches/imgui.patch
)