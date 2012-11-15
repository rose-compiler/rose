// Function arguments

extern int TAINTED;

int identity(int x) { return x; }

#ifdef INCLUDE_BROKEN_TESTS
// Fails an assertion in live/dead variable analysis even before we start trying to do taint analysis.
// liveDeadVarAnalysis.C:925: Assertion `l' failed.
int F1() {
    return identity(TAINTED);
}
#endif

#ifdef INCLUDE_BROKEN_TESTS
// The identity() call (incorrectly) sanitizes the tainted argument.  The value returned by identity() is bottom, when it
// should have been tainted.
int F2() {
    int a = TAINTED;
    return identity(a);
}
#endif


// No use having more tests yet since the most basic one fails.
