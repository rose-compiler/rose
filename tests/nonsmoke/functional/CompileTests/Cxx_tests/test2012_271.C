
void foo()
   {
  // Note: this is allowed by EDG, but not by GNU g++ (which requires "loopToHere: ;", the extra ";")
loopToHere: 
   }
