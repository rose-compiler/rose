// Example problem demonstrating error in -rose:C_Only option (didn't
// unparse the block associated with the second if statement).

int i;
void foo()
   {
     if (i != 1)
        { i = 42; }

     if (i != 1)
        i = 42;
   }
