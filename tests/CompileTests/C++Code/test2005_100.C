
void foo()
{
   int x;
   switch(x)
   {
   // Unparser puts out "case 1:" as "case 1: {}"
      case 1:
      case 2:
      {
         break;
      }
      
   }
   
}

