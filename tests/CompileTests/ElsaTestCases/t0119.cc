// t0119.cc
// check deep signature normalization

int f(int (*)(int)) {}
//ERROR(1): int f(int (*)(const int)) {}      // illegal redefinition


int f(int (*)(int *)) {}
int f(int (*)(const int *)) {}    // different
