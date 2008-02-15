// Dimonstrate case where the "case 0:" is dropped form the generated code!
int x;
void foo()
   {
     switch(x) { case 0: default: }
   }
