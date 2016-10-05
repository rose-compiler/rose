/* This is a copy of in16.c */
/* FromAEInterpC.c */
#define MCAPMSHARED
#ifdef MCAPMSHARED
#define SHARED shared
#define MCAPMCONST /**/
void *MPISM_Private(shared void *);
#define PRIVATE(x) MPISM_Private(x)
#else
#define SHARED /**/
#define MCAPMCONST const
#define PRIVATE(x) (x)
#endif

#define SAB 1


typedef struct ONEeIn_s {
   double eIn;
   SHARED double *xOutEP; /* [LxOutEP] */
} ONEeIn;

typedef struct {
   int type;
   int   LeInTabl;
   int   LxOutEP;
   SHARED ONEeIn *EPTabl; /* [LeInTabl] */
} EProb;  /* equal probable bin */
         /* output distribution */

typedef struct {
   double tau;
   int nEGlo;
   int nEGhi;
   SHARED double *xNu;
   EProb EP;
} DelayedSpectrum;

typedef struct {
    int nDelNu;
    SHARED DelayedSpectrum *Spec;      /* [nDelNu]. */
} DelayedFissionNeutrons;

typedef struct {
   int type;
   int IntType;
   SHARED EProb *cosinesEP; /* points back to corresponding cosine */
             /* distribution in parent structure */
   int   LEOutAE;
   SHARED double * SHARED * SHARED * EOutAE; /* [LeInTabl][LxOutEP][LEOutAE] */
} AngEng  ;  /* angle-energy correlated */
         /* energy distribution */



#define TWOTHIRDS 0.666666666666667

double InterpExp( double E1, double E2, double E3, double ( *userrng )( void * ), void * );


void AEInterpC( int * const nAngBin, int * const iEinLo, double * const eInAlph,
                double * const AOutAlph,
                const SHARED AngEng * const AE,
                double * EOutPart, double EXSmin,
                double ( *userrng )( void * ), void *rngstate )


/*  ver.    970221 Jim Rathkopf, X2-4602, N Division, LLNL  */

/*
  Description
   Sample and interpolate the exiting energy
   from an angle-energy correlated distribution.

   Input
     *nAngBin  - bin from which the exiting angle was sampled
                 bin is bounded by ( nAngBin,nAngBin+1 )
     *iEinLo   - low incident energy distribution from which
                 the exiting angle was interpolated
     *eInAlph  - fractional position within incident energy interval
                 'ePart' is located
     *AOutAlph - fractional position within 'nAngBin' the exiting angle
                 is located
     *AE       - angle-energy ( equal probable ) table
     EXSmin    - first energy group boundary [MeV]
     *userrng  - user supplied random number generator
     *rngstate - random number generator state

   Output
     *EOutPart - sampled particle exiting energy
*/

/* internal */
{
  int iEinHi, nAngBLo, nAngBHi, nBins, nBin;
  double EOutAlph;
  double ELoLo, EHiLo, ELoHi, EHiHi, EEinLo, EEinHi;
  double E1Lo,E2Lo,E3Lo,E1Hi,E2Hi,E3Hi;
  double c1,c2,c3,c4;
  double E1,E2,E3;
  double dAlph;
  int extexp;
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  /*
  randomly select equal probable bin
  uniformly select outgoing quantity from within bin from
   four sets:  nAngBLo,iEinLo  ( ELoLo )
               nAngBHi,iEinLo  ( EHiLo )
               nAngBLo,iEinHi  ( ELoHi )
               nAngBHi,iEinHi  ( EHiHi )
  two-way interpolate to get 'EOutPart'

  Note:  ELoLo & ELoHi ( and EHiLo & EHiHi ) are not energies
        at the same outgoing angle.  Instead, they are
        energies at the angles with the same probability
        due to the equal probable sampling method.

  ^
  ^
  prob |          |                                      |
     - nAngBHi  x  HiLo                          HiHi  x
     |          |                                      |
     |          |   eInAlph                            |
     |          |>>>>>>>>>>>>>>>|                      |
     |          |               |                      |
     - AngOut   x---------------x----------------------x
     |          |   ^                                  |
     |          |   ^                                  |
     |          |   ^ AoutAlph                         |
     |          |   ^                                  |
     |          |   ^                                  |
     |          |   ^                                  |
     - nAngBLo  x---- LoLo                       LoHi  x
     |          |                                      |
     |          |                                      |
     |          |                                      |
    -+----------|---------------|----------------------|-------
             iEinLo          eIn                    iEinHi  Ein->

  */

  iEinHi = *iEinLo + 1;
  nAngBLo = *nAngBin;
  nAngBHi = *nAngBin + 1;
  nBins = AE->LEOutAE - 1;
  /* nBins = (((AngEng *)(((char *)AE)+MPISMOFFSET))->LEOutAE) - 1; */
  extexp =0;

  if ( ( nBins%2 ) && AE->IntType == SAB )
    {
      nBins--;  /* exponent is stored in last bin */
      extexp=1;
    }

  EOutAlph = nBins*( *userrng )( rngstate );
  nBin = (int) EOutAlph;
  nBin = nBins-1;
  EOutAlph = EOutAlph - nBin;

  if ( nBin == 0 )
    {

      EEinLo = AE->EOutAE[*iEinLo][nAngBLo][1]*( 1.0-( *AOutAlph ) ) +
/* EEinLo = (((double *)(((char *)(((double **)(((char *)(((double ***)(((char *)(((AngEng *)(((char *)AE)+MPISMOFFSET))->EOutAE))+MPISMOFFSET))[*iEinLo]))+MPISMOFFSET))[nAngBLo]))+MPISMOFFSET))[1])*( 1.0-( *AOutAlph ) ) + */
               AE->EOutAE[*iEinLo][nAngBHi][1]*( *AOutAlph );
/* (((double *)(((char *)(((double **)(((char *)(((double ***)(((char *)(((AngEng *)(((char *)AE)+MPISMOFFSET))->EOutAE))+MPISMOFFSET))[*iEinLo]))+MPISMOFFSET))[nAngBHi]))+MPISMOFFSET))[1])*( *AOutAlph ); */
      EEinHi = AE->EOutAE[iEinHi][nAngBLo][1]*( 1.0-( *AOutAlph ) ) +
               AE->EOutAE[iEinHi][nAngBHi][1]*( *AOutAlph );

      E2 = ( EEinLo*( 1.0-( *eInAlph ) ) + EEinHi*( *eInAlph ) );

      do
        {

          *EOutPart = E2 * 1.0;

        }
      while ( *EOutPart < EXSmin );

    }

  else if ( nBin == ( nBins-1 ) && AE->IntType == SAB )    /* exp. falloff for S( a,b ) reactions */
/* ((AngEng *)(((char *)AE)+MPISMOFFSET))->IntType */
    {

      if ( extexp )
        {
          c1 = AE->EOutAE[*iEinLo][nAngBLo][nBin+2];
          c2 = AE->EOutAE[*iEinLo][nAngBHi][nBin+2];
          c3 = AE->EOutAE[iEinHi][nAngBLo][nBin+2];
          c4 = AE->EOutAE[iEinHi][nAngBHi][nBin+2];
        }

      if ( extexp && c1>0.0 && c2>0.0 && c3>0.0 && c4>0.0 )
        {
          /* exponents are stored in data file */

          dAlph = 1.0;

          ELoLo = AE->EOutAE[*iEinLo][nAngBLo][nBin] - dAlph/c1;
          EHiLo = AE->EOutAE[*iEinLo][nAngBHi][nBin] - dAlph/c2;
          ELoHi = AE->EOutAE[iEinHi][nAngBLo][nBin] - dAlph/c3;
          EHiHi = AE->EOutAE[iEinHi][nAngBHi][nBin] - dAlph/c4;

          /* exiting angle */
          EEinLo = ELoLo*( 1.0-( *AOutAlph ) ) + EHiLo*( *AOutAlph );
          EEinHi = ELoHi*( 1.0-( *AOutAlph ) ) + EHiHi*( *AOutAlph );

          /* incident energy */
          *EOutPart = ( EEinLo*( 1.0-( *eInAlph ) ) + EEinHi*( *eInAlph ) );

        }
      else
        {
          /* fit exponents */

          E1Lo = AE->EOutAE[*iEinLo][nAngBLo][nBin-1]*( 1.0-( *AOutAlph ) ) +
                 AE->EOutAE[*iEinLo][nAngBHi][nBin-1]*( *AOutAlph );
          E2Lo = AE->EOutAE[*iEinLo][nAngBLo][nBin]*( 1.0-( *AOutAlph ) ) +
                 AE->EOutAE[*iEinLo][nAngBHi][nBin]*( *AOutAlph );
          E3Lo = AE->EOutAE[*iEinLo][nAngBLo][nBin+1]*( 1.0-( *AOutAlph ) ) +
                 AE->EOutAE[*iEinLo][nAngBHi][nBin+1]*( *AOutAlph );

          E1Hi = AE->EOutAE[iEinHi][nAngBLo][nBin-1]*( 1.0-( *AOutAlph ) ) +
                 AE->EOutAE[iEinHi][nAngBHi][nBin-1]*( *AOutAlph );
          E2Hi = AE->EOutAE[iEinHi][nAngBLo][nBin]*( 1.0-( *AOutAlph ) ) +
                 AE->EOutAE[iEinHi][nAngBHi][nBin]*( *AOutAlph );
          E3Hi = AE->EOutAE[iEinHi][nAngBLo][nBin+1]*( 1.0-( *AOutAlph ) ) +
                 AE->EOutAE[iEinHi][nAngBHi][nBin+1]*( *AOutAlph );

          E1 = ( E1Lo*( 1.0-( *eInAlph ) ) + E1Hi*( *eInAlph ) );
          E2 = ( E2Lo*( 1.0-( *eInAlph ) ) + E2Hi*( *eInAlph ) );
          E3 = ( E3Lo*( 1.0-( *eInAlph ) ) + E3Hi*( *eInAlph ) );

          *EOutPart = InterpExp( E1, E2, E3, userrng, rngstate );

        }
    }

  else    /* flat  */
    {

      ELoLo = AE->EOutAE[*iEinLo][nAngBLo][nBin]*( 1.0-EOutAlph ) +
              AE->EOutAE[*iEinLo][nAngBLo][nBin+1]*EOutAlph;
      EHiLo = AE->EOutAE[*iEinLo][nAngBHi][nBin]*( 1.0-EOutAlph ) +
              AE->EOutAE[*iEinLo][nAngBHi][nBin+1]*EOutAlph;
      ELoHi = AE->EOutAE[iEinHi][nAngBLo][nBin]*( 1.0-EOutAlph ) +
              AE->EOutAE[iEinHi][nAngBLo][nBin+1]*EOutAlph;
      EHiHi = AE->EOutAE[iEinHi][nAngBHi][nBin]*( 1.0-EOutAlph ) +
              AE->EOutAE[iEinHi][nAngBHi][nBin+1]*EOutAlph;

      /* exiting angle */
      EEinLo = ELoLo*( 1.0-( *AOutAlph ) ) + EHiLo*( *AOutAlph );
      EEinHi = ELoHi*( 1.0-( *AOutAlph ) ) + EHiHi*( *AOutAlph );

      /* incident energy */
      *EOutPart = ( EEinLo*( 1.0-( *eInAlph ) ) + EEinHi*( *eInAlph ) );
    }

  return;

}


double InterpExp( double E1, double E2, double E3, double ( *userrng )( void * ), void *rngstate )
/* internal */
{

  double eps, f, del, T;
  double tau;
  double dAlph;

  tau = 1.0/0.6931471806;
  dAlph = ( *userrng )( rngstate );
  eps = ( E3-E2 ) / ( E2-E1 ) - 1.0;
  if ( eps > 0.0001 )     /* otherwise flat */
    {
      f = eps*( 1.0+( tau-1.0 )*eps );
      del = f / ( 1.0+tau*f );
      T = ( E2-E1 ) / del;
      dAlph = -T*1.0;
      return ( E2 + dAlph );
    }
  else
    {

      return ( E2 + dAlph*( E3-E2 ) );

    }

}

