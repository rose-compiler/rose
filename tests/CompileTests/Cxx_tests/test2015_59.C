
// DQ (9/24/2015): This code is only suited to the 4.8 version and later versions of g++.
#if ( ( (__GNUC__ == 4) && (__GNUC_MINOR__ >= 8) ) || (__GNUC__ == 5) )

struct ppackage
   {
     void* type;
   };

ppackage ppnull() 
   {
     return (ppackage) { .type = 0L };
   }

#endif
