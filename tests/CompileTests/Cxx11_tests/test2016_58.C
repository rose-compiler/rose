  typedef __attribute((vector_size(16))) int V4I;
  int g() {
    V4I v = { 1, 2, 3, 4 };
 // return v[2];  // Now accepted in some GNU modes.
  }
