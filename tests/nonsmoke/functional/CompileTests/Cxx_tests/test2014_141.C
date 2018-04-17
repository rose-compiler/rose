namespace XXX 
   {
     extern int array[];

     namespace Y 
        {

          const unsigned ArraySize = 3;
        }

     extern int array[Y::ArraySize];
   } // namespace XXX
