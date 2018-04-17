// This test code is testing both only formatting, and
// the correctness of the -rose:merge_decl_assign option.
double *space;

void foobar()
{
  for (int def = 0; def < 42; def++) {
    double cc;
    if (true) {
      double *a = space;
      double *b = space;
      for (int i = 0; i < 42; i++) {
        double aa;
        double bb;
        double abc;
        aa = a[i];
        bb = b[i];
        abc = aa * aa;
      }
    }
  }
}
