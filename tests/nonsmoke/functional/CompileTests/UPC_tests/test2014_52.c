/* This is a subset of in16.c */

#define SHARED shared

typedef struct 
   {
  /* SHARED double * SHARED * SHARED * EOutAE; */
  /* SHARED double * EOutAE; */
     SHARED double * SHARED * SHARED * EOutAE; 
   } AngEng;

/* void AEInterpC( int * const nAngBin, int * const iEinLo, double * const AOutAlph, const SHARED AngEng * const AE) */
void AEInterpC( int * const nAngBin, const SHARED AngEng * const AE)
   {
     int nAngBLo;
     double EEinLo;
     int nAngBHi;

     int * const iEinLo;
     double * const AOutAlph;

     nAngBLo = *nAngBin;

     EEinLo = AE->EOutAE[*iEinLo][nAngBLo][1]*( 1.0-( *AOutAlph ) ) +
/* EEinLo = (((double *)(((char *)(((double **)(((char *)(((double ***)(((char *)(((AngEng *)(((char *)AE)+MPISMOFFSET))->EOutAE))+MPISMOFFSET))[*iEinLo]))+MPISMOFFSET))[nAngBLo]))+MPISMOFFSET))[1])*( 1.0-( *AOutAlph ) ) + */
               AE->EOutAE[*iEinLo][nAngBHi][1]*( *AOutAlph );
/* (((double *)(((char *)(((double **)(((char *)(((double ***)(((char *)(((AngEng *)(((char *)AE)+MPISMOFFSET))->EOutAE))+MPISMOFFSET))[*iEinLo]))+MPISMOFFSET))[nAngBHi]))+MPISMOFFSET))[1])*( *AOutAlph ); */
   }


