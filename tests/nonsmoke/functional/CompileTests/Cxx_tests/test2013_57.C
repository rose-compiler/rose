// Reduced example problem from test2011_121.C (which includes #include<complex>).

void foo()
   {
     switch (1)
        {
       // This is the case of a stmt_decl as the first statement of a case statement in a switch statement.
          case 7:
               const char* day;
               break;
        }
   }
