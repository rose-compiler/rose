foo (double a, double b)
   {
  // Example of nested function, not handled by EDG (but it should work if I can turn on the correct options).
  // After some hunting in the EDG manual:
  // The following GNU extensions are not currently supported in any GNU mode:
  //  * The forward declaration of function parameters (so they can participate in variable-length array parameters).
  //  * GNU-style complex integral types (complex floating-point types are supported)
  //  * Nested functions
  //  * Local structs with variable-length array fields. Such fields are treated (with a warning) as zero-length arrays in
  //       GNU C mode, which is a useful approximation in some circumstances, but not generally equivalent to the GNU
  //       feature.

     double square (double z) { return z * z; }

     return square (a) + square (b);
   }
