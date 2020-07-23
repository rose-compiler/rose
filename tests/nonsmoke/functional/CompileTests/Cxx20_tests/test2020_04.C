unsigned int i = 1;
auto r = -1 < i; // existing pitfall: returns ‘false’

// DQ (7/21/2020): Failing case should not be tested
// auto r2 = -1 <=> i; // Error: narrowing conversion required

