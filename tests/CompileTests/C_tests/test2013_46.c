// const int *sample_fmts_alt = (const int[]) { 2,3 };
// int *sample_fmts_alt = (int[]) { 2,3 };

// This is (I think) not a case of a compound literal (since it lacks the type specification)...
int *sample_fmts_alt = { -2,3 };
