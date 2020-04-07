
#------------------------------------------------------------------------------
# This cmake file enables the AddressSanitizer
# see http://clang.llvm.org/docs/AddressSanitizer.html 
#     http://en.wikipedia.org/wiki/AddressSanitizer

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  # add compiler flag
  # -> requires clang > 3.1 or gcc > 4.8
  if (MSVC)
    message(WARNING "AddressSanitizer can only be enabled for GCC and Clang")
  else()
    # add AddressSanitizer also for compiler
    add_definitions( -fsanitize=address
                     -fno-omit-frame-pointer)
    # add AddressSanitizer also for linker
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fsanitize=address")
  endif()
else()
  message(WARNING "AddressSanitizer is supported for SmartPeak debug mode only")
endif()

