## define some source directories
set(CF_SMARTPEAK_DATA_PATH ${SMARTPEAK_HOST_DIRECTORY}/share/SmartPeak CACHE INTERNAL "Path to the shared documents of SmartPeak.")
set(CF_SMARTPEAK_DOC_PATH ${SMARTPEAK_HOST_DIRECTORY}/doc CACHE INTERNAL "Path to the documentation of SmartPeak.")
## and the corresponding ones when installed
set(CF_SMARTPEAK_INSTALL_DATA_PATH ${INSTALL_SHARE_DIR} CACHE INTERNAL "Path to the installed shared documents of SmartPeak.")
set(CF_SMARTPEAK_INSTALL_DOC_PATH ${INSTALL_DOC_DIR} CACHE INTERNAL "Path to the installed documentation of SmartPeak." )

## check for Microsoft Visual Studio compiler
if (MSVC)
	set(SMARTPEAK_COMPILER_MSVC "1" CACHE INTERNAL "Do we use Microsoft Compiler?")
endif()
## check for G++
if (CMAKE_COMPILER_IS_GNUCXX)
	set(SMARTPEAK_COMPILER_GXX "1" CACHE INTERNAL "Do we use G++ Compiler?")
endif()

INCLUDE(TestBigEndian)
TEST_BIG_ENDIAN(SMARTPEAK_BIG_ENDIAN)

## check 32/64 bit architecture (defined above!)
if (NOT DEFINED SMARTPEAK_64BIT_ARCHITECTURE)
	message(FATAL_ERROR "Cmake script was re-ordered and is now invalid! Please make sure that SMARTPEAK_64BIT_ARCHITECTURE is defined when config.h.in is configured!")
endif()

include(CheckTypeSize) ## Check sizeof a type
CHECK_TYPE_SIZE("unsigned char" SIZE_UCHAR)
CHECK_TYPE_SIZE("unsigned short" SIZE_USHORT)
CHECK_TYPE_SIZE("unsigned int" SIZE_UINT)
CHECK_TYPE_SIZE("unsigned long" SIZE_ULONG)
CHECK_TYPE_SIZE("unsigned long long" SIZE_ULONGLONG)
CHECK_TYPE_SIZE("short" SIZE_SHORT)
CHECK_TYPE_SIZE("int" SIZE_INT)
CHECK_TYPE_SIZE("long" SIZE_LONG)
CHECK_TYPE_SIZE("long long" SIZE_LONGLONG)

CHECK_TYPE_SIZE("int32_t" SIZE_INT32)
if (HAVE_SIZE_INT32)
	set(CF_SMARTPEAK_INT32_TYPE int32_t)
else()
	## search for another Int32 type
	if (SIZE_INT MATCHES "4")
		set(CF_SMARTPEAK_INT32_TYPE int)
	elseif (SIZE_SHORT MATCHES "4")
		set(CF_SMARTPEAK_INT32_TYPE short)
	elseif (SIZE_LONG MATCHES "4")
		set(CF_SMARTPEAK_INT32_TYPE long)
	else()
		Message(FATAL_ERROR "Cannot find signed 32bit integer type. Please contact the developers!")
	endif()
endif()

CHECK_TYPE_SIZE("int64_t" SIZE_INT64)
if (HAVE_SIZE_INT64)
	set(CF_SMARTPEAK_INT64_TYPE int64_t)
else()
	## search for another Int64 type
	if (SIZE_INT MATCHES "8")
		set(CF_SMARTPEAK_INT64_TYPE int)
	elseif (SIZE_LONG MATCHES "8")
		set(CF_SMARTPEAK_INT64_TYPE long)
	elseif (SIZE_LONGLONG MATCHES "8")
		set(CF_SMARTPEAK_INT64_TYPE "long long")
	else()
		Message(FATAL_ERROR "Cannot find signed 64bit integer type. Please contact the developers!")
	endif()
endif()

CHECK_TYPE_SIZE("uint8_t" SIZE_UINT8)
if (HAVE_SIZE_UINT8)
	set(CF_SMARTPEAK_BYTE_TYPE uint8_t)
else()
	## search for another uint8 type
	if (SIZE_UCHAR MATCHES "1")
		set(CF_SMARTPEAK_BYTE_TYPE "unsigned char")
	elseif (SIZE_USHORT MATCHES "1")
		set(CF_SMARTPEAK_BYTE_TYPE "unsigned short")
	else()
		Message(FATAL_ERROR "Cannot find unsigned 8bit integer (byte) type. Please contact the developers!")
	endif()
endif()


CHECK_TYPE_SIZE("uint32_t" SIZE_UINT32)
if (HAVE_SIZE_UINT32)
	set(CF_SMARTPEAK_UINT32_TYPE uint32_t)
else()
	## search for another UInt32 type
	if (SIZE_UINT MATCHES "4")
		set(CF_SMARTPEAK_UINT32_TYPE "unsigned int")
	elseif (SIZE_USHORT MATCHES "4")
		set(CF_SMARTPEAK_UINT32_TYPE "unsigned short")
	elseif (SIZE_ULONG MATCHES "4")
		set(CF_SMARTPEAK_UINT32_TYPE "unsigned long")
	else()
		Message(FATAL_ERROR "Cannot find unsigned 32bit integer type. Please contact the developers!")
	endif()
endif()

CHECK_TYPE_SIZE("uint64_t" SIZE_UINT64)
if (HAVE_SIZE_UINT64)
	set(CF_SMARTPEAK_UINT64_TYPE uint64_t)
else()
	## search for another uint64 type
	if (SIZE_ULONG MATCHES "8")
		set(CF_SMARTPEAK_UINT64_TYPE "unsigned long")
	elseif (SIZE_ULONGLONG MATCHES "8")
		set(CF_SMARTPEAK_UINT64_TYPE "unsigned long long")
	else()
		Message(FATAL_ERROR "Cannot find uint64 type. Please contact the developers!")
	endif()
endif()

#------------------------------------------------------------------------------
## Check if various system heards exist
include(CheckIncludeFileCXX)

CHECK_INCLUDE_FILE_CXX("unistd.h" SMARTPEAK_HAS_UNISTD_H)
CHECK_INCLUDE_FILE_CXX("process.h" SMARTPEAK_HAS_PROCESS_H)

CHECK_INCLUDE_FILE_CXX("time.h" SMARTPEAK_HAS_TIME_H)
CHECK_INCLUDE_FILE_CXX("sys/types.h" SMARTPEAK_HAS_SYS_TYPES_H)
CHECK_INCLUDE_FILE_CXX("sys/times.h" SMARTPEAK_HAS_SYS_TIMES_H)
CHECK_INCLUDE_FILE_CXX("sys/time.h"  SMARTPEAK_HAS_SYS_TIME_H)
CHECK_INCLUDE_FILE_CXX("stdint.h"  SMARTPEAK_HAS_STDINT_H)

#------------------------------------------------------------------------------
# check for libc++ bug
try_run(_stream_bug_run_result_var _stream_bug_compile_var
        ${CMAKE_BINARY_DIR}
        ${SMARTPEAK_HOST_DIRECTORY}/cmake/modules/check_string_stream_bug.cxx)

# set stream variable
if(NOT _stream_bug_run_result_var)
  set(SMARTPEAK_HAS_STREAM_EXTRACTION_BUG "1")
endif()

#------------------------------------------------------------------------------
# check if certain c++ functions exist
include(CheckFunctionExists)
## in MinGW we have the signal.h header, but no kill() as in Linux, so we need to check for the kill() function
CHECK_FUNCTION_EXISTS("kill" SMARTPEAK_HAS_KILL)
CHECK_FUNCTION_EXISTS("sysconf" SMARTPEAK_HAS_SYSCONF)


#------------------------------------------------------------------------------
# Create the config.h
# replace any variables in config.h.in with current values
set (CONFIGURED_CONFIG_H ${PROJECT_BINARY_DIR}/include/SmartPeak/config.h)
configure_file(${PROJECT_SOURCE_DIR}/include/SmartPeak/config.h.in ${CONFIGURED_CONFIG_H})

#------------------------------------------------------------------------------
# Create smartpeak_package_version.h
# replace any variables in smartpeak_package_version.h.in with current values
set (CONFIGURED_SMARTPEAK_PACKAGE_VERSION_H ${PROJECT_BINARY_DIR}/include/SmartPeak/smartpeak_package_version.h)
configure_file(${PROJECT_SOURCE_DIR}/include/SmartPeak/smartpeak_package_version.h.in ${CONFIGURED_SMARTPEAK_PACKAGE_VERSION_H})

#------------------------------------------------------------------------------
# create paths header
set(CONFIGURED_SMARTPEAK_DATA_PATH_H ${PROJECT_BINARY_DIR}/include/SmartPeak/smartpeak_data_path.h)
configure_file(${PROJECT_SOURCE_DIR}/include/SmartPeak/smartpeak_data_path.h.in ${CONFIGURED_SMARTPEAK_DATA_PATH_H})

#------------------------------------------------------------------------------
# export a list of all configured heders
set(SmartPeak_configured_headers "${CONFIGURED_CONFIG_H};${CONFIGURED_SMARTPEAK_PACKAGE_VERSION_H};${CONFIGURED_SMARTPEAK_DATA_PATH_H}")
