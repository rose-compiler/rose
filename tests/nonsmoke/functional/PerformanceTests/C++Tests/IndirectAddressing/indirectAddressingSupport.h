

class IndirectAddressingSupport
   {
     double* generateDataArray(int size);
     int* generateIndirectionArray(int size);

     void randomizeInitialization ( int* indirectionArray, int size );
     int* copyIndirectionArray ( int* indirectionArray, int size );

     void sortIndirectionArray( int* indirectionArray, int size );
   };

