# --------------------------------------------------------------------------
#   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
# --------------------------------------------------------------------------
# Copyright The SmartPeak Team -- Novo Nordisk Foundation 
# Center for Biosustainability, Technical University of Denmark 2018-2022.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
# INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# --------------------------------------------------------------------------
# $Maintainer: Ahmed Khalil, Bertrand Boudaud $
# $Authors: Douglas McCloskey $
# --------------------------------------------------------------------------
# --------------------------------------------------------------------------
# gRPC
# --------------------------------------------------------------------------
option(FETCHCONTENT_QUIET OFF)
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
# set(ABSL_LOCAL_GOOGLETEST_DIR ${googletest-distribution_SOURCE_DIR})
set(absl_gtest_download_url "https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip")
include(FetchContent)
FetchContent_Declare(
  gRPC
  GIT_REPOSITORY https://github.com/grpc/grpc.git
  GIT_TAG        v1.41.0
  GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(gRPC)

set(grpc_required_libs 
    libprotobuf grpc++ 
    grpc++_reflection 
    grpc++_unsecure grpc_unsecure
    grpc++_error_details
    grpc gpr
    absl_flags
    crypto
    address_sorting
    grpc_plugin_support grpcpp_channelz)
