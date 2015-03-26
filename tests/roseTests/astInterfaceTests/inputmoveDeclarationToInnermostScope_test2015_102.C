
// This is an example of a statment split over two macros, the better fix is to not do this.
// This problem was fixed in the original applcation code (ptlmix2d.cc).
#define MACRO_IF if(0)
#define MACRO_BODY { i = 0; }

void foobar()
   {
     int i;

     MACRO_IF
     MACRO_BODY

   }
