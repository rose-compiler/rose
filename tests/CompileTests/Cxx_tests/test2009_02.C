// TOO (2/15/2011): for Thrifty g++ 3.4.4:
// "error: invalid register name `edx' for register variable"
#if !( __GNUC__ == 3 && __GNUC_MINOR__ == 4 )
void foo()
   {
  // EDG Bug: Now that we have the asm modifier, the referenced register is always "ax"!
  // register int value  asm ("bx") = 7;
     register int value  asm ("edx") = 7;
   }
#endif
