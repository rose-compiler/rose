struct ppackage
   {
     void* type;
   };

ppackage ppnull() 
   {
     ppackage p = { 0L };

     return p;
   }
