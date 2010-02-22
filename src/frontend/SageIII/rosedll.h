#ifndef __ROSEDLL__
#define __ROSEDLL__

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define ROSE_HELPER_DLL_IMPORT __declspec(dllimport)
  #define ROSE_HELPER_DLL_EXPORT __declspec(dllexport)
  #define ROSE_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define ROSE_HELPER_DLL_IMPORT __attribute__ ((visibility("default")))
    #define ROSE_HELPER_DLL_EXPORT __attribute__ ((visibility("default")))
    #define ROSE_HELPER_DLL_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define ROSE_HELPER_DLL_IMPORT
    #define ROSE_HELPER_DLL_EXPORT
    #define ROSE_HELPER_DLL_LOCAL
  #endif
#endif


//#ifdef ROSE_ROSETTA // defined if ROSE_ROSETTA is compiled as a DLL
  #ifdef ROSE_ROSETTA_EXPORTS // defined if we are building the ROSE_ROSETTA DLL (instead of using it)
    #define ROSE_ROSETTA_API ROSE_HELPER_DLL_EXPORT
//    #warning "ROSE_ROSETTA_API : ROSE_HELPER_DLL_EXPORT"
  #else
    #define ROSE_ROSETTA_API ROSE_HELPER_DLL_IMPORT
//    #warning "ROSE_ROSETTA_API : ROSE_HELPER_DLL_IMPORT"
  #endif // ROSE_ROSETTA_DLL_EXPORTS
  #define ROSE_ROSETTA_LOCAL ROSE_HELPER_DLL_LOCAL
//#else // ROSE_ROSETTA_DLL is not defined: this means ROSE_ROSETTA is a static lib.
//  #define ROSE_ROSETTA_API
//  #define ROSE_ROSETTA_LOCAL
//  #warning "ROSE_ROSETTA_API : NULL"
//#endif // ROSE_ROSETTA_DLL



#endif

