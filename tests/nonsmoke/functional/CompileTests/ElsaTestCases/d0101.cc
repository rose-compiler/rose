// more implicit-int ambiguity problems

// /home/ballB/XFree86-4.2.0-8/LabMnL-1Zss.i:11197:44:
// WARNING: there is no action to merge nonterm DirectAbstractDeclarator
double g(double a);
int f() {
  float hue;
  (g (((hue))));
}

// Scott's example of implicit int causing unnecessary parsing
// ambiguities.
int foo (int ((x))) {}
