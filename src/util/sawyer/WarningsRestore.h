// Do not protect this file with include-once macros.

// This file is the counterpart of <sawyer/WarningsOff.h> and restores warnings to their state before WarningsOff was
// included. The WarningsRestore should be included once per WarningsOff -- they nest like parentheses.

#ifdef SAWYER_CONFIGURED
#    if _MSC_VER
#        pragma warning(pop)
#    endif
#else
#    error "The <sawyer/Sawyer.h> file must have been included already."
#endif
