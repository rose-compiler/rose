// This test code demonstrates a bug in the interaction of Python with the g++ header files
// It didn't have previously when we used to patch the header files.

// The Fix: The fix to this problem was to remove the special version of Python.h that we used to
// have to maintain when ROSE was not as GNU compatable.  Great news, since this simplifies ROSE.

// Original Code:
//    extern void DBShowErrors(int,void (*)(char *));

// Generated code:
//    extern "C" { void DBShowErrors(int ,void (*)(std::allocator < char > ::pointer )); }

#include <Python.h>

extern void DBShowErrors(int,void (*)(char *));
