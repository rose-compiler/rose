#ifndef GENERAL_SETUP_H
#define GENERAL_SETUP_H

// DQ (3/8/2006): We can't redefine C++ keywords
// #ifndef Boolean
// #define Boolean int
// #endif
// #define false 0
// #define true  1
// #endif

// JJW (8/26/2008): Removing these
// #define FALSE 0
// #define TRUE 1

#if 0
#ifndef INLINE
#define INLINE inline
#endif
#endif

class PreprocessingInfo;

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

#endif  /* general_h */
