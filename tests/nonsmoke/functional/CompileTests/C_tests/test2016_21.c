
// const int *sample_fmts_alt = (const int[]) { 2,3 };
// int *sample_fmts_alt = (int[]) { 2,3 };
const int x = 9;

void foobar()
   {
  // NOTE: This is non-standard C code (and not reproduced in ROSE to be exactly the same).
  // This is (I think) not a case of a compound literal (since it lacks the type specification)...
     int *sample_fmts_alt = { x,3 };
   }
