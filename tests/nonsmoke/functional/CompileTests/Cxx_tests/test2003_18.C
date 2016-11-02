
void foo();
void foobar(void)
   {
     switch (0)
        {
          case 0:
          foo(); 
          break;
          case 1:
          break;
          default:
       // Note that gnu appears to require "{}"
             {}
        }
   }

