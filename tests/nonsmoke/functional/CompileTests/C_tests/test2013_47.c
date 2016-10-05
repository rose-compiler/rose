// const int *sample_fmts_alt = (const int[]) { 2,3 };
// int *sample_fmts_alt = (int[]) { 2,3 };

// This fails in ROSE...
// This is (I think) not a case of a compound literal (since it lacks the type specification 
// of an array)...I think it is just a cast of someting that is incomplete (or an array).
// I am not really clear what this is, but it appears to be accepted by EDG for C code.
int *sample_fmts_alt = (int) { 2,3 };
