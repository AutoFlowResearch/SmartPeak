include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)
set (EXTRA_CMAKE_ARGS)

set (BOOST_VERSION  1.67.0)
message (STATUS "Adding Boost ${BOOST_VERSION} as an external project.")

if (WIN32)
    set (BOOST_BOOTSTRAP_CMD bootstrap.bat)
    set (BOOST_BUILD_CMD b2.exe)
elseif (UNIX)
    set (BOOST_BOOTSTRAP_CMD ./bootstrap.sh)
    set (BOOST_BUILD_CMD ./b2)
endif ()


if (CMAKE_SIZEOF_VOID_P MATCHES "8")
  set(BOOST_ADDR_MODEL 64 CACHE INTERNAL "Architecture-bits")
  set(BOOST_ARCH_MODEL "address-model=64")
else()
  set(BOOST_ADDR_MODEL 32 CACHE INTERNAL "Architecture-bits")
  set(BOOST_ARCH_MODEL "")
endif()
message(STATUS "ADDRESSMODEL is: ${BOOST_ADDR_MODEL}-bit")
if (NOT BOOST_ADDR_MODEL MATCHES "32|64")
  Message(FATAL_ERROR "BOOST_ADDR_MODEL is neither 32 nor 64! Please correct this!")
endif()

if (MSVC)
#if (WIN32)
  ## check that the console environment has a cl.exe architecture which is identical to the VS Generator
  ## If cl.exe builds 32-bit libs and VS Generator is Win64, we'd end up with mixed 32bit/64bit libraries, depending on how each lib is build (Cmake, bjam, nmake)
  execute_process(COMMAND "cl.exe" OUTPUT_VARIABLE cl_out ERROR_VARIABLE cl_out)
  if (cl_out MATCHES ".*x64.*")
    message(STATUS "Cl.exe produces: 64-bit")
    set(CL_ADDRESSMODEL 64)
  elseif (cl_out MATCHES ".*x86.*")
    message(STATUS "Cl.exe produces: 32-bit")
    set(CL_ADDRESSMODEL 32)
  else()
    message(FATAL_ERROR "Could not determine if cl.exe builds x86 or x64 apps. Make sure cl.exe is available in your environment!")
  endif()
  if (NOT (CL_ADDRESSMODEL EQUAL BOOST_ADDR_MODEL))
    message(FATAL_ERROR "cl.exe (${CL_ADDRESSMODEL}-bit) and Visual Studio Generator (${BOOST_ADDR_MODEL}-bit) do not match. Please fix your PATH environment to find the proper cl.exe or use/omit the Win64 generator.")
  endif()

  if (CMAKE_GENERATOR MATCHES ".*Visual Studio 8 2005.*")
    set(BOOST_TOOLSET msvc-8.0)
  elseif (CMAKE_GENERATOR MATCHES ".*Visual Studio 9 2008.*")
    set(BOOST_TOOLSET msvc-19.0)
  elseif (CMAKE_GENERATOR MATCHES ".*Visual Studio 10.*")
    set(BOOST_TOOLSET msvc-10.0)
  elseif (CMAKE_GENERATOR MATCHES ".*Visual Studio 11.*")
    set(BOOST_TOOLSET msvc-11.0)
  elseif (CMAKE_GENERATOR MATCHES ".*Visual Studio 12.*")
    set(BOOST_TOOLSET msvc-12.0)
  elseif (CMAKE_GENERATOR MATCHES ".*Visual Studio 14.*")
    set(BOOST_TOOLSET msvc-14.0)
  elseif (CMAKE_GENERATOR MATCHES ".*Visual Studio 15.*")
    set(BOOST_TOOLSET msvc-15.0)
  else()
    if (OVERRIDE_GENERATOR)
      message(FATAL_ERROR "Chosen to override the Generator check, proceed with caution.")
    else()
      message(FATAL_ERROR "Please use 'Visual Studio ?? [Win64]' (??={8 2005, 9 2008, 10, 11, 12, 14, 15}) as Generator - identical to the MSVC toolchain you plan to use! There will be errors (mostly missing libraries). Under very special circumstances, you can override this with -DOVERRIDE_GENERATOR=On.")
    endif()
  endif()

  set (BOOST_BUILD_ARGS address-model=${BOOST_ADDR_MODEL} ${BOOST_ARCH_MODEL} --with-test toolset=${BOOST_TOOLSET} variant=release link=static --prefix=${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Source/boost/build install)

else() ## linux/macos
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
      if(APPLE)
        set(BOOST_TOOLSET "clang-darwin")
      else()
        set(_boost_bootstrap_toolchain "clang")
        set(BOOST_TOOLSET "clang")
      endif()
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU*")
      if(APPLE)
        ## For Apples old GCC (tag in lib name will be xgcc)
        set(BOOST_TOOLSET "darwin") 
      else()
        set(BOOST_TOOLSET "gcc")
      endif()
    endif()

    set (BOOST_BUILD_ARGS --with-test --prefix=${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Source/boost/build install)
endif()

ExternalProject_Add(boost
    #PREFIX ${CMAKE_CURRENT_BINARY_DIR}/boost
    GIT_REPOSITORY "https://github.com/boostorg/boost.git"
    GIT_TAG boost-${BOOST_VERSION}
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ${BOOST_BOOTSTRAP_CMD}
    BUILD_IN_SOURCE 1
    BUILD_COMMAND ${BOOST_BUILD_CMD} ${BOOST_BUILD_ARGS}
    INSTALL_COMMAND ""
    GIT_PROGRESS 1
    LOG_DOWNLOAD 1
    LOG_UPDATE 1
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_INSTALL 1
)

list (APPEND EXTRA_CMAKE_ARGS
  -DBOOST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Source/boost
  -DBoost_NO_SYSTEM_PATHS=ON)