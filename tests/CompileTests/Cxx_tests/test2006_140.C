void foo()
   {
     char hexadecimal = '\x42';
     char octal = '\22';
     char decimal = 67;
     unsigned char unsignedDecimal = 255;
     signed char positiveSignedChar = 127;
     signed char negativeSignedChar = -128;

#if defined(__clang__)
  // I think that Clang is correct to require this, since it is a single character hex literal.
     char long_hexadecimal = '\x42';
#else
     char long_hexadecimal = '\x424242424242424242424242424242';
#endif
   }
