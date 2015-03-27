struct list_head {};

struct range 
   {
     struct list_head list;
   };

void find_range()
   {
    struct range *y;
    for (y = ({ typeof( ((typeof(*y) *)0)->list ) *__mptr = 0L; (typeof(*y) *) ( (char *)__mptr - 0L );}); 
         0;
         y++)
    {
       break;
    }
}
