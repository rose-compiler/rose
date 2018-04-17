
class X
   {
     public:
          int* buffer;
          int numbnds;
          int ndims;

          void foobar();
   };

void
X::foobar()
   {
     int num_items;

     if (ndims == 2) 
        {
          num_items = 5 * numbnds;
        } 
       else 
        {
          num_items = 7 * numbnds;
        }

     buffer = new int[num_items];
   }

