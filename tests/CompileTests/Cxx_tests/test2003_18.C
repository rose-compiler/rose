
void foo();
void foobar(void)
   {
  // Note that unused cases will not be output!
     switch (0)
        {
          case 0:
          foo(); 
          break;
          case 1:
          break;
          default:
        }
   }

