void foo ()
   {
     class A {} x1;

  // BUG: the TRUE statment is output in the function scope and the true branch is empty.
     if (true)
          class A {} x3; 
   }

