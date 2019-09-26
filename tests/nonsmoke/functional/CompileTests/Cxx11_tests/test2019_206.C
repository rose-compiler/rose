
void foobar() 
   {
  // This builds 3 SgClassDeclaration IR nodes, but one of them is names B (incorrect) and the other are unnamed (correct).
     typedef struct {} B;

  // B x;
   }
