// Arrays of pointer conversion fixes: Qualification conversions and pointers to arrays of pointers

typedef int *A[3];               // array of 3 pointer to int
typedef const int *const CA[3];  // array of 3 const pointer to const int

CA &&r = A{}; // ok, reference binds to temporary array object after qualification conversion to type CA

// A &&r1 = const_cast<A>(CA{});   // error: temporary array decayed to pointer

A &&r2 = const_cast<A&&>(CA{}); // ok

