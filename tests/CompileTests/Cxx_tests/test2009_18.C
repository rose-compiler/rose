// DQ (7/27/2009): This test is part of fixing why the AST Whole Graph 
// generation does not look into (output) expression statements.

void foo()
   {
     int x;
     x = 1 + 2;
   }
