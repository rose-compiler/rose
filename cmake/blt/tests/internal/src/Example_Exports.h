// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#ifndef _EXAMPLE_EXPORTS_H_
#define _EXAMPLE_EXPORTS_H_

//-----------------------------------------------------------------------------
// -- define proper lib exports for various platforms --
//-----------------------------------------------------------------------------
#if defined(_WIN32)
  #if defined(WIN32_SHARED_LIBS)
    #if defined(EXAMPLE_EXPORTS) || defined(example_EXPORTS)
      #define EXAMPLE_API __declspec(dllexport)
    #else
      #define EXAMPLE_API __declspec(dllimport)
    #endif
  #else
    #define EXAMPLE_API /* not needed for static on windows */
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
  #if __GNUC__ >= 4 && (defined(EXAMPLE_EXPORTS) || defined(example_EXPORTS))
    #define EXAMPLE_API __attribute__ ((visibility("default")))
  #else
    #define EXAMPLE_API /* hidden by default */
  #endif
#endif

#endif
