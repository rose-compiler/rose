// Testing the source position of #pragma IR nodes.

void foobar()
   {
     for (;;)
        #pragma rose "test"
        #pragma
        #pragma rose
        break;
   }
