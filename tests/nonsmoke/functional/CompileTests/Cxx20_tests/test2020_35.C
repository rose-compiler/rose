

// DQ (7/21/2020): constinit appears to not be implemented in EDG 6.0.
extern thread_local constinit int x;

int f() { return x; } // no check of a guard variable needed

