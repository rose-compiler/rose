
// Debug source position details of case statements:
enum values {yes, no };


void foo()
   {
     int x = 0;
     values question = yes;
     switch (question)
        {
       // The block in each case is compiler generated and not suppressed properly in code generation.
          case yes: x = 41; break;
          case no: x = 42; break;
        }
   }
