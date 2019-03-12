# - Try to find ImGui lib
#
# This module supports requiring a minimum version, e.g. you can do
#   find_package(ImGui 3.1.2)
# to require version 3.1.2 or newer of Eigen3.
#
# Once done this will define
#
#  IMGUI_FOUND - system has ImGui lib with correct version
#  IMGUI_INCLUDE_DIR - the ImGui include directory
#  IMGUI_VERSION - ImGui version

if (IMGUI_INCLUDE_DIR)

  # in cache already
  set(IMGUI_FOUND "YES")

else (IMGUI_INCLUDE_DIR)

  find_path(IMGUI_INCLUDE_DIR NAMES imgui.h)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(ImGUI DEFAULT_MSG IMGUI_INCLUDE_DIR)

  mark_as_advanced(IMGUI_INCLUDE_DIR)
  set(IMGUI_FOUND "YES")

endif(IMGUI_INCLUDE_DIR)

if (IMGUI_FOUND)
  add_library(ImGUI INTERFACE IMPORTED)
  #include_directories(${IMGUI_INCLUDE_DIR})
endif (IMGUI_FOUND)

