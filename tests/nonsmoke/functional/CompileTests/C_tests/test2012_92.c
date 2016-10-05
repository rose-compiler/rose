
// This code fails at the 2nd case statment...
void XXX_bin_print()
   {
     int x;

     switch (x) 
        {
          case 400:
               if (1) 42;
//        break;
     case 401:
#if 1
//   case 402:
         43; // break;
         44; // break;
#endif

//   default:
// Added this but it is not relevant to the faulure..
//        break;

        }
   }


