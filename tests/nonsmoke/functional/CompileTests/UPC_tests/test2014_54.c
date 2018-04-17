/* This is a copy of in16.c */
#define SHARED shared

typedef struct 
   {
     int   LEOutAE;
   } AngEng;

void AEInterpC( const SHARED AngEng * const AE )
   {
     int nBins;
     nBins = AE->LEOutAE - 1;
  /* nBins = (((AngEng *)(((char *)AE)+MPISMOFFSET))->LEOutAE) - 1; */
   }

// Commenting this out causes the shared pointer transforamtion to be
// correct.  Uncommenting this code causes twice the number of transformations
// as are expected.
#if 1
void InterpExp()
   {
   }
#endif

