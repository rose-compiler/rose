/* This is a subset of in16.c */

#define SHARED shared

typedef struct 
   {
     int   LEOutAE;
   } AngEng;

void AEInterpC( const SHARED AngEng * const AE)
   {
     int nBins;

     nBins = AE->LEOutAE - 1;
  /* nBins = (((AngEng *)(((char *)AE)+MPISMOFFSET))->LEOutAE) - 1; */
   }
