
#ifndef SMARTPEAK_CONFIG_H
#define SMARTPEAK_CONFIG_H

// include SMARTPEAK_DLLAPI macros
#include <SmartPeak/SmartPeakConfig.h>

#include <boost/current_function.hpp>

// Here are some global configuration flags for SmartPeak

// Define compiler specifics (used in VERY few places only)
// Microsoft Visual Studio .NET, 2005, 2008
#define SMARTPEAK_COMPILER_MSVC
// GNU g++
/* #undef SMARTPEAK_COMPILER_GXX */

// __PRETTY_FUNCTION__ is a GNU G++ extension so we use the alternative in boost
#define SMARTPEAK_PRETTY_FUNCTION BOOST_CURRENT_FUNCTION

// SMARTPEAK_ASSERTIONS enables some debugging methods within some SmartPeak classes
#ifdef SMARTPEAK_COMPILER_MSVC
// we define this using NDEBUG on MSVC as there are multiple build types simultaneously in the Solution file,
// thus setting one value will not fit them all
#	ifndef NDEBUG // hopefully defined automatically by MS-compiler in Debug Mode
#		define SMARTPEAK_ASSERTIONS
#	endif
#else // linux & Co (only one build type at a time)
#  if (1)
#    define SMARTPEAK_ASSERTIONS
#  endif
#endif


// let Cmake decide if we are using Windows (i.e. if windows.h is available).
//   GCC and MSVC have pre-defined macros for this as well but using -ansi in GCC will disable those, thus asking the compiler is fragile
#ifndef WIN32  //avoid warning of redefinition
#define WIN32
#endif

#ifdef WIN32   //should be true on: MinGW (32bit + 64bit) & MSVS compiler
#define SMARTPEAK_WINDOWSPLATFORM 1
#endif

// are we building a shared lib?
/* #undef BUILD_SHARED_LIBS */

/* #undef SMARTPEAK_BIG_ENDIAN */

// Define on 64 bit architectures
#define SMARTPEAK_64BIT_ARCHITECTURE

#define SMARTPEAK_INT32_TYPE int32_t
#define SMARTPEAK_INT64_TYPE int64_t
#define SMARTPEAK_BYTE_TYPE uint8_t
#define SMARTPEAK_UINT32_TYPE uint32_t
#define SMARTPEAK_UINT64_TYPE uint64_t
//#define SMARTPEAK_SIZE_T_SIGNED 

// if you ever want to do abs() or floor() on this type in VC then use _abs64() and include <stdlib.h> (no kidding!)
#define PointerSizeInt int64_t
#define PointerSizeUInt uint64_t

/* #undef SMARTPEAK_HAS_UNISTD_H */
#define SMARTPEAK_HAS_PROCESS_H
#define SMARTPEAK_HAS_STDINT_H

#define SMARTPEAK_HAS_TIME_H
#define SMARTPEAK_HAS_SYS_TYPES_H
/* #undef SMARTPEAK_HAS_SYS_TIMES_H */
/* #undef SMARTPEAK_HAS_SYS_TIME_H */

/* #undef SMARTPEAK_HAS_KILL */
/* #undef SMARTPEAK_HAS_SYSCONF */

/* #undef ENABLE_UPDATE_CHECK */

// is libc++ or has stream bug
/* #undef SMARTPEAK_HAS_STREAM_EXTRACTION_BUG */

#define SMARTPEAK_BOOST_VERSION_MAJOR 
#define SMARTPEAK_BOOST_VERSION_MINOR 
#define SMARTPEAK_BOOST_VERSION_SUBMINOR 
#define SMARTPEAK_BOOST_VERSION 

#define COINOR_SOLVER 

#define SMARTPEAK_GLPK_VERSION 
#define SMARTPEAK_GLPK_VERSION_MAJOR 
#define SMARTPEAK_GLPK_VERSION_MINOR 

// class tests

#ifdef _OPENMP
  #define IF_MASTERTHREAD if (omp_get_thread_num() ==0)
#else
  #define IF_MASTERTHREAD
#endif

#endif // SMARTPEAK_CONFIG_H
