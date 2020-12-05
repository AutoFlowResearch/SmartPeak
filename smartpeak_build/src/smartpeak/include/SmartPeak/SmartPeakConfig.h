
#ifndef SMARTPEAK_DLLAPI_H
#define SMARTPEAK_DLLAPI_H

#ifdef SMARTPEAK_STATIC_DEFINE
#  define SMARTPEAK_DLLAPI
#  define SMARTPEAK_NO_EXPORT
#else
#  ifndef SMARTPEAK_DLLAPI
#    ifdef SmartPeak_EXPORTS
        /* We are building this library */
#      define SMARTPEAK_DLLAPI 
#    else
        /* We are using this library */
#      define SMARTPEAK_DLLAPI 
#    endif
#  endif

#  ifndef SMARTPEAK_NO_EXPORT
#    define SMARTPEAK_NO_EXPORT 
#  endif
#endif

#ifndef SMARTPEAK_DEPRECATED
#  define SMARTPEAK_DEPRECATED __declspec(deprecated)
#endif

#ifndef SMARTPEAK_DEPRECATED_EXPORT
#  define SMARTPEAK_DEPRECATED_EXPORT SMARTPEAK_DLLAPI SMARTPEAK_DEPRECATED
#endif

#ifndef SMARTPEAK_DEPRECATED_NO_EXPORT
#  define SMARTPEAK_DEPRECATED_NO_EXPORT SMARTPEAK_NO_EXPORT SMARTPEAK_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef SMARTPEAK_NO_DEPRECATED
#    define SMARTPEAK_NO_DEPRECATED
#  endif
#endif

#endif /* SMARTPEAK_DLLAPI_H */
