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

#include <rosePublicConfig.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Binary analysis features
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

// ARM AArch64 A64 instructions (Sage nodes, disassembly, unparsing, semantics, etc.)
#if !defined(ROSE_ENABLE_ASM_A64) && __cplusplus >= 201103L && defined(ROSE_HAVE_CAPSTONE)
    #define ROSE_ENABLE_ASM_A64
#endif

// Whether to enable concolic testing.
#if !defined(ROSE_ENABLE_CONCOLIC_TESTING) && \
    __cplusplus >= 201103L && \
    (defined(ROSE_HAVE_SQLITE3) || defined(ROSE_HAVE_LIBPQXX)) && \
    BOOST_VERSION >= 106400
#define ROSE_ENABLE_CONCOLIC_TESTING
#endif

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C/C++ analysis features
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
