

// If clang is defined then __GNUC__ should be defined.
#ifdef __INTEL_COMPILER
   #ifndef __GNUG__
      #error "Intel compiler defines __GNUG__."
   #endif
   #ifndef __GNUC__
      #error "Intel compiler defines __GNUC__."
   #endif
   #ifndef __GNUC_MINOR__
      #error "Intel compiler defines __GNUC_MINOR__."
   #endif
   #ifndef __GNUC_PATCHLEVEL__
      #error "Intel compiler defines __GNUC_PATCHLEVE__."
   #endif
   #ifdef __clang__
      #error "Intel compiler can't defined with __clang__ defined."
   #endif
#endif


// If clang is defined then __GNUC__ should be defined.
#ifdef __clang__
   #ifndef __GNUG__
      #error "Clang compiler is defined and GNU compiler should be specified using __GNUG__."
   #endif
   #ifndef __GNUC__
      #error "Clang compiler is defined and GNU compiler should be specified using __GNUC__."
   #endif
   #ifndef __GNUC_MINOR__
      #error "Clang compiler is defined and GNU compiler should be specified using __GNUC_MINOR__."
   #endif
   #ifndef __GNUC_PATCHLEVEL__
      #error "Clang compiler is defined and GNU compiler should be specified using __GNUC_PATCHLEVEL__."
   #endif
   #ifdef __INTEL_COMPILER
      #error "Clang compiler can't defined with __INTEL_COMPILER defined."
   #endif

#if 0
// DQ (5/16/2016): Turn this on when this detail is fixed in ROSE's clang support.
   #if (__GNUG__ != 4 || __GNUC__ != 4 || __GNUC_MINOR__ != 2 || __GNUC_PATCHLEVEL__ != 1)
      #error "Clang compiler not defined with correct GNU 4.2.1 version number."
   #endif
#endif
#endif

#if !defined(__INTEL_COMPILER) && !defined(__clang__)
   #ifndef __GNUG__
      #error "GNU compiler is defined and should be specified using __GNUG__."
   #endif
   #ifndef __GNUC__
      #error "GNU compiler is defined and should be specified using __GNUC__."
   #endif
   #ifndef __GNUC_MINOR__
      #error "GNU compiler is defined and should be specified using __GNUC_MINOR__."
   #endif
   #ifndef __GNUC_PATCHLEVEL__
      #error "GNU compiler is defined and should be specified using __GNUC_PATCHLEVEL__."
   #endif
#endif
