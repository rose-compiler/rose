
void foo()
   {
  // EDG Bug: Now that we have the asm modifier, the referenced register is always "ax"!
  // register int value  asm ("bx") = 7;
     register int value  asm ("edx") = 7;
   }
