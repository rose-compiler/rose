// This file defines various C preprocessor macros that describe what ROSE features are enabled in this configuration. The
// advantage of doing this in a C header instead of calculating it in the configuration system and storing a result is that
// this same logic can then be used across all configuration and build systems.
//
// All ROSE feature macros start with the letters "ROSE_ENABLED_" followed by the name of the feature. These macros are defined
// if the feature is enabled, and not defined (but also not #undef) if the feature is not automatically enabled. By not
// explicitly undefining the macro, we make it possible for developers to enabled features from the C++ compiler command-line
// that would not normally be enabled.

#ifndef ROSE_FeatureTests_H
#define ROSE_FeatureTests_H

// DO NOT INCLUDE LARGE HEADERS HERE! These headers should generally be only C preprocessor directives, not any substantial
// amount of C++ code. This means no sage3basic.h or rose.h, among others. This <featureTests.h> file is meant to be as small
// and fast as possible because its purpose is to be able to quickly compile (by skipping over) source code that's not
// necessary in a particular ROSE configuration.
#include <rosePublicConfig.h>
#include <boost/version.hpp>

#if defined(_MSC_VER)
    // Microsoft Visual C++ Compiler erroneously advertises that it's a C++98 compiler. The "fix" for Visual Studio 2017
    // version 15.7 Preview 3 is not really a fix -- the user has to explicitly opt-in on the command-line that __cplusplus
    // should be set to the correct value. Therefore, we just avoid checking this compiler and let the user try to figure out
    // that they're using the wrong language based on the compiler's error messages.
    //   See https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=msvc-160
    //   See https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
#elif __cplusplus < 201103L
    #error "ROSE requires a C++11 or later compiler"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Binary analysis features
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

// A more traditional name than ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#define ROSE_ENABLE_BINARY_ANALYSIS

// ARM AArch64 A64 instructions (Sage nodes, disassembly, unparsing, semantics, etc.)
#if !defined(ROSE_ENABLE_ASM_AARCH64) && defined(ROSE_HAVE_CAPSTONE)
    #define ROSE_ENABLE_ASM_AARCH64
#endif

// ARM AArch32 instructions (Sage nodes, disassembly, unparsing, semantics, etc.)
// Instruction semantics for AArch32 requires C++17 or later.
#if !defined(ROSE_ENABLE_ASM_AARCH32) && __cplusplus >= 201703L && defined(ROSE_HAVE_CAPSTONE)
    #define ROSE_ENABLE_ASM_AARCH32
#endif

// Whether to enable concolic testing.
#if !defined(ROSE_ENABLE_CONCOLIC_TESTING) && \
    defined(__linux__) && \
    __cplusplus >= 201402L && \
    (defined(ROSE_HAVE_SQLITE3) || defined(ROSE_HAVE_LIBPQXX)) && \
    BOOST_VERSION >= 106400 && \
    defined(ROSE_HAVE_BOOST_SERIALIZATION_LIB)
#define ROSE_ENABLE_CONCOLIC_TESTING
#endif

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Source code analysis in general.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Defined if any source-level analysis is enabled.
#if !defined(ROSE_ENABLE_SOURCE_ANALYSIS) && ( \
    defined(ROSE_BUILD_CPP_LANGUAGE_SUPPORT) ||   \
    defined(ROSE_BUILD_CUDA_LANGUAGE_SUPPORT) || \
    defined(ROSE_BUILD_CXX_LANGUAGE_SUPPORT) || \
    defined(ROSE_BUILD_C_LANGAUGE_SUPPORT) || \
    defined(ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT) || \
    defined(ROSE_BUILD_JAVA_LANGUAGE_SUPPORT) || \
    defined(ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT) || \
    defined(ROSE_BUILD_PHP_LANGUAGE_SUPPORT) || \
    defined(ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT) || \
    defined(ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION) || \
    defined(ROSE_EXPERIMENTAL_COBOL_ROSE_CONNECTION) || \
    defined(ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION) || \
    defined(ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION) || \
    defined(ROSE_EXPERIMENTAL_MATLAB_ROSE_CONNECTION) || \
    defined(ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION))
#define ROSE_ENABLE_SOURCE_ANALYSIS
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C/C++ analysis features
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
