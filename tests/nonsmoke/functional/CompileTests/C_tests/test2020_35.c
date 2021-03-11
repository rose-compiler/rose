// Note that the order of the operands is different and the types don't match correctly.
void foo (var1, var2)
   unsigned var2;
   unsigned *var1;
   {
  // Original code:  *var1 = var2;
  // Generated code: *var2 = var1;
     *var1 = var2;
   }

