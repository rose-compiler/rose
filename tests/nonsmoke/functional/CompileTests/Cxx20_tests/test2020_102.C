int n = 42;

// DQ (7/21/2020): Failing case should not be tested
// double a[n][5]; // error

auto p1 = new  double[n][5];  // OK

// DQ (7/21/2020): Failing case should not be tested
// auto p2 = new  double[5][n];  // error: only the first dimension may be non-constant
// auto p3 = new (double[n][5]); // error: syntax (1) cannot be used for dynamic arrays

