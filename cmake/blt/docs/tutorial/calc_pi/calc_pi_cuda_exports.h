//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// calc_pi_cuda_exports.h
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#ifndef CALC_PI_CUDA_EXPORTS_H
#define CALC_PI_CUDA_EXPORTS_H

//-----------------------------------------------------------------------------
// -- define proper lib exports for various platforms -- 
//-----------------------------------------------------------------------------
#if defined(_WIN32)
    #if defined(WIN32_SHARED_LIBS)
        #if defined(CALC_PI_CUDA_EXPORTS) || defined(calc_pi_cuda_EXPORTS)
            #define CALC_PI_CUDA_API __declspec(dllexport)
        #else
            #define CALC_PI_CUDA_API __declspec(dllimport)
        #endif
    #else
        #define CALC_PI_CUDA_API /* not needed for static on windows */
    #endif
    #if defined(_MSC_VER)
        /* Turn off warning about lack of DLL interface */
        #pragma warning(disable:4251)
        /* Turn off warning non-dll class is base for dll-interface class. */
        #pragma warning(disable:4275)
        /* Turn off warning about identifier truncation */
        #pragma warning(disable:4786)
    #endif
#else
# if __GNUC__ >= 4 && (defined(CALC_PI_CUDA_EXPORTS) || defined(calc_pi_cuda_EXPORTS))
#   define CALC_PI_CUDA_API __attribute__ ((visibility("default")))
# else
#   define CALC_PI_CUDA_API /* hidden by default */
# endif
#endif

#endif
