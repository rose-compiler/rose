void duff( int count )
   {
     int* to;
     int* from;
     register int n = (count + 7) / 8;      /* count > 0 assumed */

     switch (count % 8)
        {
//        case 0: do { *to = *from++;
                  do { *to = *from++;
          case 7:      *to = *from++;
          case 6:      *to = *from++;
          case 5:      *to = *from++;
          case 4:      *to = *from++;
          case 3:      *to = *from++;
          case 2:      *to = *from++;
          case 1:      *to = *from++;
                     } while (--n > 0);
//                count++;
//                count++;
//                count++;
        }
   }

