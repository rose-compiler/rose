
// const int *sample_fmts_alt = (const int[]) { 2,3 };
// int *sample_fmts_alt = (int[]) { 2,3 };
const int x = 9;

#if defined(__EDG_VERSION__) && __EDG_VERSION__ == 49
// Only literal values are allowed in the generated code and using EDG 4.9 
// this code was normalized to literals, but using EDG 4.11 and great there
// is no such normalization and so it generates non-standard C code (same as
// the input).
// This is (I think) not a case of a compound literal (since it lacks the type specification)...
int *sample_fmts_alt = { x,3 };
#endif
