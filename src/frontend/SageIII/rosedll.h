#ifndef __ROSEDLL__
#define __ROSEDLL__

/******************************************************************
 * tps (02/24/2010)
 * A DLL file has a layout very similar to an .exe file, with one important difference — 
 * a DLL file contains an exports table. The exports table contains the name of every function 
 * that the DLL exports to other executables. These functions are the entry points into the DLL; 
 * only the functions in the exports table can be accessed by other executables. Any other functions 
 * in the DLL are private to the DLL.
 *
 * To export a function from a DLL use the keyword : __declspec(dllexport) in the functions definition
 *
 * A program that uses public symbols defined by a DLL is said to import them. When you create header files 
 * for applications that use your DLLs to build with, use __declspec(dllimport) on the declarations 
 * of the public symbols. The keyword __declspec(dllimport) works with the __declspec(dllexport) keyword.
 * 
 * You can use the same header file for both the DLL and the client application. 
 * To do this, use a special preprocessor symbol that indicates whether you are building the DLL or
 * building the client application. To defined if you export a symbol use the library name and then _EXPORTS,
 * for ROSE this would be ROSE_DLL_EXPORTS. I.e. when you compile the ROSE DLL, ROSE_DLL_EXPORTS is defined
 * and when you compile the user code that uses the ROSE DLL, ROSE_DLL_IMPORTS is defined.
 * 
 * In the configuration below ROSE_DLL_API is defined empty space as default. Special cases are:
 *  1) Windows: then __declspec(dllimport/export) are used
 *  2) Linux - GNUC >=4 and ROSE not compiling ROSE: then the visibility attribute is used to set
 *             all symbols hidden - to simulate the effect that occurs under Windows in Linux
 ******************************************************************/

#if defined _WIN32 || defined __CYGWIN__
  #define ROSE_DLL_HELPER_DLL_IMPORT __declspec(dllimport)
  #define ROSE_DLL_HELPER_DLL_EXPORT __declspec(dllexport)
  #define ROSE_DLL_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4 && !defined(USE_ROSE)
    #define ROSE_DLL_HELPER_DLL_IMPORT __attribute__ ((visibility("default")))
    #define ROSE_DLL_HELPER_DLL_EXPORT __attribute__ ((visibility("default")))
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
  #else
    #define ROSE_DLL_API ROSE_DLL_HELPER_DLL_IMPORT
  #endif // ROSE_DLL_DLL_EXPORTS
  #define ROSE_DLL_LOCAL ROSE_DLL_HELPER_DLL_LOCAL


// DQ (10/19/2010): Need to test if we can remove this.
// We should not reference CXX_IS_ROSE_ANALYSIS except in source code.
// tps : this is probably not needed anymore
// undef ROSE_ROSETTA_API if rose analyses itself.
// #if CXX_IS_ROSE_ANALYSIS
//  #undef ROSE_DLL_API
//  #define ROSE_DLL_API
//#endif 


#endif

