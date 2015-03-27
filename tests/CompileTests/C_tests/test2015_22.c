struct list_head {};

struct range 
   {
     struct list_head list;
   };

void find_range()
   {
     struct range *y;

  // for (y = ({ typeof( ((struct range *)0)->list ) *__mptr = 0L; (struct range *) ( (char *)__mptr - 0L );}); 
  // for (y = ({ struct list_head *__mptr = 0L; (struct range *) ( (char *)__mptr - 0L );}); 
     for (y = ({ struct list_head *__mptr = 0L; (struct range *) ( (char *)__mptr - 0L );}); 
          0;
          y++)
        {
          break;
        }
   }
