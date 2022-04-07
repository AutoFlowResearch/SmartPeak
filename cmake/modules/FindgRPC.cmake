#.rst:
# Find gRPC
# ---------
#
# Finds the gRPC library. This module defines:
#
#  gRPC_FOUND                - True if gRPC is found
#
# Additionally these variables are defined for internal usage:
#
#  GRPC_INCLUDE_DIR          - Include dir 
#
# If ``gRPC`` is not found, as a fallback it tries to find the C++ sources.
# You can supply their location via an ``IMGUI_DIR`` variable.
#
# The find module tries to find ``gRPC`` the C++ sources.
# You can supply their location via an ``GRPC_DIR`` variable.\ ly.
#set(CMAKE_FIND_DEBUG_MODE TRUE)
find_path(GRPC_INCLUDE_DIR 
	NAMES "grpc/grpc.h"
	HINTS ${gRPC_DIR}
	NO_CMAKE_FIND_ROOT_PATH
)
mark_as_advanced(GRPC_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(gRPC REQUIRED_VARS GRPC_INCLUDE_DIR HANDLE_COMPONENTS)