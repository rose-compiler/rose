
/* This is a copy of in16.c */

#define SHARED shared

typedef struct 
   {
     SHARED double * SHARED * SHARED * EOutAE;
   } AngEng;

void AEInterpC( int * const nAngBin, int * const iEinLo, double * const AOutAlph, const SHARED AngEng * const AE)
   {
     int nAngBLo;
     double EEinLo;

     nAngBLo = *nAngBin;

  /* Wanted: EEinLo = (((double *)(((char *)(((double **)(((char *)(((double ***)(((char *)(((AngEng *)(((char *)AE)+MPISMOFFSET))->EOutAE))+MPISMOFFSET))[*iEinLo]))+MPISMOFFSET))[nAngBLo]))+MPISMOFFSET))[1])*( 1.0-( *AOutAlph ) ) + */
  /* Bug:    EEinLo = ((double *)(((char *)((double **)(((char *)((double ***)(((char *)(AE -> EOutAE)) + MPISMOFFSET))[ *iEinLo]) + MPISMOFFSET))[nAngBLo]) + MPISMOFFSET))[1] * (1.0 -  *AOutAlph); */

     EEinLo = AE->EOutAE[*iEinLo][nAngBLo][1]*( 1.0-( *AOutAlph ) );
   }


