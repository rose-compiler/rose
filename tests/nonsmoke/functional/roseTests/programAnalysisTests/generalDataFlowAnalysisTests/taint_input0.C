// Simple sources of taint

extern int TAINTED_g1;
static int TAINTED_g2;
int TAINTED_g3;

int A1() { return TAINTED_g1; }
int A2() { return TAINTED_g2; }
int A3() { return TAINTED_g3; }

int B1() { extern int TAINTED_l1; return TAINTED_l1; }
int B2() { static int TAINTED_l2; return TAINTED_l2; }
int B3() { int TAINTED_l3=0; return TAINTED_l3; }

int C1() { return A1(); }
int C2() { return A2(); }
int C3() { return A3(); }
