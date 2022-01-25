#.rst:
# Find PortableFileDialogs
# -------------
#
# Finds the PortableFileDialogs library. This module defines:
#
#  PortableFileDialogs_FOUND                - True if PortableFileDialogs is found
#
# Additionally these variables are defined for internal usage:
#
#  PORTABLEFILEDIALOGS_INCLUDE_DIR          - Include dir 
#
# If ``PortableFileDialogs`` is not found, as a fallback it tries to find the C++ sources.
# You can supply their location via an ``IMGUI_DIR`` variable.
#
# The find module tries to find ``PortableFileDialogs`` the C++ sources.
# You can supply their location via an ``PORTABLEFILEDIALOGS_DIR`` variable.\ ly.
set(CMAKE_FIND_DEBUG_MODE TRUE)
find_path(PORTABLEFILEDIALOGS_INCLUDE_DIR 
	NAMES "portable-file-dialogs.h"
	HINTS ${PORTABLEFILEDIALOGS_DIR}
	NO_CMAKE_FIND_ROOT_PATH
)
mark_as_advanced(PORTABLEFILEDIALOGS_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PortableFileDialogs REQUIRED_VARS PORTABLEFILEDIALOGS_INCLUDE_DIR HANDLE_COMPONENTS)