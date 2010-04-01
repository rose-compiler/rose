#ifndef __ROSEDLL__
#define __ROSEDLL__

// tps (02/24/2010)
// For Linux, ROSE_API is defined as ROSE_HELPER_DLL_IMPORT
// which is always empty space
// Actually, the default is empty space: ROSE_DLL_EXPORTS is only defined
// when using cmake. If not used, ROSE_DLL_API = ROSE_DLL_HELPER_DLL_IMPORT = EMPTY SPACE


// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define ROSE_DLL_HELPER_DLL_IMPORT __declspec(dllimport)
  #define ROSE_DLL_HELPER_DLL_EXPORT __declspec(dllexport)
  #define ROSE_DLL_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4 && !defined(USE_ROSE)
    #define ROSE_DLL_HELPER_DLL_IMPORT __attribute__ ((visibility("default")))
//    #define ROSE_DLL_HELPER_DLL_IMPORT 
    #define ROSE_DLL_HELPER_DLL_EXPORT __attribute__ ((visibility("default")))
//    #define ROSE_DLL_HELPER_DLL_EXPORT 
    #define ROSE_DLL_HELPER_DLL_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define ROSE_DLL_HELPER_DLL_IMPORT
    #define ROSE_DLL_HELPER_DLL_EXPORT
    #define ROSE_DLL_HELPER_DLL_LOCAL
  #endif
#endif


// ROSE_DLL_EXPORTS is only defined for cmake
  #ifdef ROSE_DLL_EXPORTS // defined if we are building the ROSE DLL (instead of using it)
    #define ROSE_DLL_API ROSE_DLL_HELPER_DLL_EXPORT
//    #warning "ROSE_DLL_API : ROSE_DLL_HELPER_DLL_EXPORT"
  #else
// this is the default for automake
    #define ROSE_DLL_API ROSE_DLL_HELPER_DLL_IMPORT
//    #warning "ROSE_DLL_API : ROSE_DLL_HELPER_DLL_IMPORT"
  #endif // ROSE_DLL_DLL_EXPORTS
  #define ROSE_DLL_LOCAL ROSE_DLL_HELPER_DLL_LOCAL

// undef ROSE_ROSETTA_API if rose analyses itself. 
#if CXX_IS_ROSE_ANALYSIS
  #undef ROSE_DLL_API
  #define ROSE_DLL_API
#endif 


#endif

