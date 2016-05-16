
#ifdef __GNUC__
   #ifdef __INTEL_COMPILER
      #error "Intel compiler is defined when GNU compiler is specified."
   #endif
#else
   #ifdef __GNUC_MINOR__
      #error "GNU is not defined, but __GNUC_MINOR__ is defined."
   #endif
   #ifdef __GNUC_PATCHLEVEL__
      #error "GNU is not defined, but __GNUC_PATCHLEVEL__ is defined."
   #endif
#endif

// If clang is defined then __GNUC__ should be defined.
#ifdef __INTEL_COMPILER
   #ifdef __GNUC__
      #error "Intel compiler is defined when GNU compiler is specified."
   #endif
#endif


// If clang is defined then __GNUC__ should be defined.
#ifdef __clang__
   #ifndef __GNUC__
      #error "Clang compiler is defined and GNU compiler should be specified using __GNUC__."
   #endif
   #ifndef __GNUC_MINOR__
      #error "Clang compiler is defined and GNU compiler should be specified using __GNUC_MINOR__."
   #endif
   #ifndef __GNUC_PATCHLEVEL__
      #error "Clang compiler is defined and GNU compiler should be specified using __GNUC_PATCHLEVEL__."
   #endif
#endif

