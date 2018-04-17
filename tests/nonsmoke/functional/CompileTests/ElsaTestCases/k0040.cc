// instantiating a template with unnamed template parameter

// originally found in package akregator_1.0-beta8-2

// Assertion failed: key != NULL, file vptrmap.cc line 58

// ERR-MATCH: \QAssertion failed: key != NULL, file vptrmap\E

template <class> struct S1;

typedef S1<int> s1;

