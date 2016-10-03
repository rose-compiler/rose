// t0013.c
// multiply-defined function

int f(int) { return 1; }

//ERROR(1): int f(int) { return 2; }
