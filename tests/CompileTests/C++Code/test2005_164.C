/*
Bug submitted by Andreas.

This code is output (generated):
int main()
{
  L250:
  ANOTHERLABL:
  return 0;
}

 */

void foo()
   {
     L250:
      ;
     ANOTHER_LABEL:
      ;
      
      
   }

void foobar()
   {
     int x = 0;
     switch(x)
        {
          case 0:
          case 1:
               break;
          case 2:
          case 3:
             {
               break;
             }

          default:
             {
            // Nothing to do here!
             }
        }
   }

int main()
   {
     L250:
          ;
     ANOTHER_LABEL:
          ;
     return 0;
   }

