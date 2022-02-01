include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)
set (GRPC_VERSION v1.41.0)
message (STATUS "Adding gRPC ${GRPC_VERSION} as an external project.")

option(abseil-cpp_BUILD_TESTS OFF)
option(gRPC_BUILD_TESTS OFF)
option(ABSL_BUILD_TESTING OFF)
option(ABSL_RUN_TESTS OFF) 
option(protobuf_BUILD_TESTS OFF)
option(gRPC_BUILD_CSHARP_EXT OFF)
option(gRPC_BUILD_GRPC_CSHARP_PLUGIN OFF)
option(gRPC_BUILD_GRPC_NODE_PLUGIN OFF)
option(gRPC_BUILD_GRPC_JAVA_PLUGIN OFF)
option(gRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN OFF)
option(gRPC_BUILD_GRPC_PHP_PLUGIN OFF)
option(gRPC_BUILD_GRPC_PYTHON_PLUGIN OFF)
option(gRPC_BUILD_GRPC_RUBY_PLUGIN OFF)
option(ABSL_USE_GOOGLETEST_HEAD ON)
option(ABSL_USE_EXTERNAL_GOOGLETEST ON)

ExternalProject_Add(gRPC
  GIT_REPOSITORY https://github.com/grpc/grpc.git
  GIT_TAG        ${GRPC_VERSION}
  GIT_PROGRESS   TRUE
  INSTALL_COMMAND ""
)
