struct ppackage
   {
     void* type;
   };

ppackage ppnull() 
   {
     return (ppackage) { .type = 0L };
   }
