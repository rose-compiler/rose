
/* Restrict macros for various compilers */
#ifdef __GNUC__
#define RESTRICT __restrict

#elif defined __KCC
#define RESTRICT restrict

#else
#define RESTRICT

#endif


#define LOOP_IMPLEMENTATION 2
#define VALIDITY_CHECK 0

#define PC(ii,jj,kk) \
ii + (jj)*jp + (kk)*kp

#define COEFF_POINTER_INCREMENT(i) \
dbl +=i; dbc +=i; dbr +=i; dcl +=i; dcc +=i; dcr +=i; dfl +=i; dfc +=i; dfr +=i;\
cbl +=i; cbc +=i; cbr +=i; ccl +=i; ccc +=i; ccr +=i; cfl +=i; cfc +=i; cfr +=i;\
ubl +=i; ubc +=i; ubr +=i; ucl +=i; ucc +=i; ucr +=i; ufl +=i; ufc +=i; ufr +=i;

#define POINTER_INCREMENT(i) \
dbl +=i; dbc +=i; dbr +=i; dcl +=i; dcc +=i; dcr +=i; dfl +=i; dfc +=i; dfr +=i;\
cbl +=i; cbc +=i; cbr +=i; ccl +=i; ccc +=i; ccr +=i; cfl +=i; cfc +=i; cfr +=i;\
ubl +=i; ubc +=i; ubr +=i; ucl +=i; ucc +=i; ucr +=i; ufl +=i; ufc +=i; ufr +=i;\
xdbl +=i; xdbc +=i; xdbr +=i; xdcl +=i; xdcc +=i; xdcr +=i; xdfl +=i; xdfc +=i; xdfr +=i;\
xcbl +=i; xcbc +=i; xcbr +=i; xccl +=i; xccc +=i; xccr +=i; xcfl +=i; xcfc +=i; xcfr +=i;\
xubl +=i; xubc +=i; xubr +=i; xucl +=i; xucc +=i; xucr +=i; xufl +=i; xufc +=i; xufr +=i; \
lhs +=i;

#define POINTER_INCREMENT_ONE \
dbl++; dbc++; dbr++; dcl++; dcc++; dcr++; dfl++; dfc++; dfr++; cbl++; cbc++; \
cbr++; ccl++; ccc++; ccr++; cfl++; cfc++; cfr++; ubl++; ubc++; ubr++; ucl++; \
ucc++; ucr++; ufl++; ufc++; ufr++; xdbl++; xdbc++; xdbr++; xdcl++; xdcc++; xdcr++; \
xdfl++; xdfc++; xdfr++; xcbl++; xcbc++; xcbr++; xccl++; xccc++; xccr++; xcfl++; xcfc++; \
xcfr++; xubl++; xubc++; xubr++; xucl++; xucc++; xucr++; xufl++; xufc++; xufr++; lhs++;

extern void perfometer();

void rmatmult3( int imin, int imax, int jmin, int jmax, int kmin, int kmax, int jp, int kp,
  double * RESTRICT dbl,
  double * RESTRICT dbc,
  double * RESTRICT dbr,
  double * RESTRICT dcl,
  double * RESTRICT dcc,
  double * RESTRICT dcr,
  double * RESTRICT dfl,
  double * RESTRICT dfc,
  double * RESTRICT dfr,
  double * RESTRICT cbl,
  double * RESTRICT cbc,
  double * RESTRICT cbr,
  double * RESTRICT ccl,
  double * RESTRICT ccc,
  double * RESTRICT ccr,
  double * RESTRICT cfl,
  double * RESTRICT cfc,
  double * RESTRICT cfr,
  double * RESTRICT ubl,
  double * RESTRICT ubc,
  double * RESTRICT ubr,
  double * RESTRICT ucl,
  double * RESTRICT ucc,
  double * RESTRICT ucr,
  double * RESTRICT ufl,
  double * RESTRICT ufc,
  double * RESTRICT ufr,
  double * RESTRICT x,
  double * RESTRICT b )
{

  int m;
  char *me = "rmatmult3" ;
  int i, ii, jj, kk ;
  double * RESTRICT lhs = b;
  double * RESTRICT xdbl = x - kp - jp - 1 ;
  double * RESTRICT xdbc = x - kp - jp     ;
  double * RESTRICT xdbr = x - kp - jp + 1 ;
  double * RESTRICT xdcl = x - kp      - 1 ;
  double * RESTRICT xdcc = x - kp          ;
  double * RESTRICT xdcr = x - kp      + 1 ;
  double * RESTRICT xdfl = x - kp + jp - 1 ;
  double * RESTRICT xdfc = x - kp + jp     ;
  double * RESTRICT xdfr = x - kp + jp + 1 ;
  double * RESTRICT xcbl = x      - jp - 1 ;
  double * RESTRICT xcbc = x      - jp     ;
  double * RESTRICT xcbr = x      - jp + 1 ;
  double * RESTRICT xccl = x           - 1 ;
  double * RESTRICT xccc = x               ;
  double * RESTRICT xccr = x           + 1 ;
  double * RESTRICT xcfl = x      + jp - 1 ;
  double * RESTRICT xcfc = x      + jp     ;
  double * RESTRICT xcfr = x      + jp + 1 ;
  double * RESTRICT xubl = x + kp - jp - 1 ;
  double * RESTRICT xubc = x + kp - jp     ;
  double * RESTRICT xubr = x + kp - jp + 1 ;
  double * RESTRICT xucl = x + kp      - 1 ;
  double * RESTRICT xucc = x + kp          ;
  double * RESTRICT xucr = x + kp      + 1 ;
  double * RESTRICT xufl = x + kp + jp - 1 ;
  double * RESTRICT xufc = x + kp + jp     ;
  double * RESTRICT xufr = x + kp + jp + 1 ;
  
  double myflops = 0.0 ;
  static int firstPass = 1;
  register double temp;
  
/*   perfometer(); */
  /* On the first pass into this function for decks/zrad.0001, if ii=imin, b[i] = 1.0, otherwise, b[i]=6.075507e-13.
     We can use this information to create a correctness check on new loop implementations.
  */
  
#if LOOP_IMPLEMENTATION == 1
  
  for ( kk = kmin ; kk < kmax ; kk++ ) {
    for ( jj = jmin ; jj < jmax ; jj++ ) {
      for ( ii = imin ; ii < imax ; ii++ ) {
        i = ii + jj * jp + kk * kp ;

        b[i] =
          dbl[i] * xdbl[i] + dbc[i] * xdbc[i] + dbr[i] * xdbr[i] +
          dcl[i] * xdcl[i] + dcc[i] * xdcc[i] + dcr[i] * xdcr[i] +
          dfl[i] * xdfl[i] + dfc[i] * xdfc[i] + dfr[i] * xdfr[i] +
          cbl[i] * xcbl[i] + cbc[i] * xcbc[i] + cbr[i] * xcbr[i] +
          ccl[i] * xccl[i] + ccc[i] * xccc[i] + ccr[i] * xccr[i] +
          cfl[i] * xcfl[i] + cfc[i] * xcfc[i] + cfr[i] * xcfr[i] +
          ubl[i] * xubl[i] + ubc[i] * xubc[i] + ubr[i] * xubr[i] +
          ucl[i] * xucl[i] + ucc[i] * xucc[i] + ucr[i] * xucr[i] +
          ufl[i] * xufl[i] + ufc[i] * xufc[i] + ufr[i] * xufr[i] ;

#if VALIDITY_CHECK == 1
        if(firstPass==1)
        {
          if(ii==imin)
          {
            if( fabs(b[i]-1.0) > 1.0e-5 )
            {
              printf("b[%d,%d,%d]=%e, should be 1.000000\n",ii,jj,kk,b[i]);
              abort();
            }
          }
          else
          {
            if( fabs(b[i] - 6.075507e-13) > 1.0e-5)
            {
              printf("b[%d,%d,%d]=%e, should be 6.075507e-13\n",ii,jj,kk,b[i]);
              abort();
            }
          }
        }
#endif /* end of VALIDITY_CHECK */
        
      }/* end of ii loop */
    }/* end of jj loop */
  }/* end of kk loop */
  
#if VALIDITY_CHECK == 1
  if(firstPass==1)
    firstPass=0;
#endif
  
#elif LOOP_IMPLEMENTATION == 2
  
  for ( kk = kmin ; kk < kmax ; kk++ ) {
    for ( jj = jmin ; jj < jmax ; jj++ ) {
      for ( ii = imin ; ii < imax ; ii++ ) {

        b[PC(ii,jj,kk)] =
          dbl[PC(ii,jj,kk)] * x[PC(ii-1,jj-1,kk-1)] +
          dbc[PC(ii,jj,kk)] * x[PC(ii,jj-1,kk-1)] +
          dbr[PC(ii,jj,kk)] * x[PC(ii+1,jj-1,kk-1)] +
          dcl[PC(ii,jj,kk)] * x[PC(ii-1,jj,kk-1)] +
          dcc[PC(ii,jj,kk)] * x[PC(ii,jj,kk-1)] +
          dcr[PC(ii,jj,kk)] * x[PC(ii+1,jj,kk-1)] +
          dfl[PC(ii,jj,kk)] * x[PC(ii-1,jj+1,kk-1)] +
          dfc[PC(ii,jj,kk)] * x[PC(ii,jj+1,kk-1)] +
          dfr[PC(ii,jj,kk)] * x[PC(ii+1,jj+1,kk-1)] +
          cbl[PC(ii,jj,kk)] * x[PC(ii-1,jj-1,kk)] +
          cbc[PC(ii,jj,kk)] * x[PC(ii,jj-1,kk)] +
          cbr[PC(ii,jj,kk)] * x[PC(ii+1,jj-1,kk)] +
          ccl[PC(ii,jj,kk)] * x[PC(ii-1,jj,kk)] +
          ccc[PC(ii,jj,kk)] * x[PC(ii,jj,kk)] +
          ccr[PC(ii,jj,kk)] * x[PC(ii+1,jj,kk)] +
          cfl[PC(ii,jj,kk)] * x[PC(ii-1,jj+1,kk)] +
          cfc[PC(ii,jj,kk)] * x[PC(ii,jj+1,kk)] +
          cfr[PC(ii,jj,kk)] * x[PC(ii+1,jj+1,kk)] +
          ubl[PC(ii,jj,kk)] * x[PC(ii-1,jj-1,kk+1)] +
          ubc[PC(ii,jj,kk)] * x[PC(ii,jj-1,kk+1)] +
          ubr[PC(ii,jj,kk)] * x[PC(ii+1,jj-1,kk+1)] +
          ucl[PC(ii,jj,kk)] * x[PC(ii-1,jj,kk+1)] +
          ucc[PC(ii,jj,kk)] * x[PC(ii,jj,kk+1)] +
          ucr[PC(ii,jj,kk)] * x[PC(ii+1,jj,kk+1)] +
          ufl[PC(ii,jj,kk)] * x[PC(ii-1,jj+1,kk+1)] +
          ufc[PC(ii,jj,kk)] * x[PC(ii,jj+1,kk+1)] +
          ufr[PC(ii,jj,kk)] * x[PC(ii+1,jj+1,kk+1)] ;

#if VALIDITY_CHECK == 1
        if(firstPass==1)
        {
          if(ii==imin)
          {
            if( fabs(b[PC(ii,jj,kk)]-1.0) > 1.0e-5 )
            {
              printf("b[%d,%d,%d]=%e, should be 1.000000\n",ii,jj,kk,b[PC(ii,jj,kk)]);
              abort();
            }
          }
          else
          {
            if( fabs(b[PC(ii,jj,kk)] - 6.075507e-13) > 1.0e-5)
            {
              printf("b[%d,%d,%d]=%e, should be 6.075507e-13\n",ii,jj,kk,b[PC(ii,jj,kk)]);
              abort();
            }
          }
        }
#endif /* end of VALIDITY_CHECK */
        
      }
    }
  } 

#if VALIDITY_CHECK == 1
  if(firstPass==1)
    firstPass=0;
#endif
  
#elif LOOP_IMPLEMENTATION == 3
  
  POINTER_INCREMENT( kmin*kp );
  
  for ( kk = kmin ; kk < kmax ; kk++ ) {
    
    POINTER_INCREMENT( jmin*jp );
    
    for ( jj = jmin ; jj < jmax ; jj++ ) {
      
      POINTER_INCREMENT( 2 );
      
      for ( ii = imin ; ii < imax ; ii++ ) {

        *lhs = (*dbl++) * (*xdbl++) + (*dbc++) * (*xdbc++) + (*dbr++) * (*xdbr++) +
          (*dcl++) * (*xdcl++) + (*dcc++) * (*xdcc++) + (*dcr++) * (*xdcr++) +
          (*dfl++) * (*xdfl++) + (*dfc++) * (*xdfc++) + (*dfr++) * (*xdfr++) +
          (*cbl++) * (*xcbl++) + (*cbc++) * (*xcbc++) + (*cbr++) * (*xcbr++) +
          (*ccl++) * (*xccl++) + (*ccc++) * (*xccc++) + (*ccr++) * (*xccr++) +
          (*cfl++) * (*xcfl++) + (*cfc++) * (*xcfc++) + (*cfr++) * (*xcfr++) +
          (*ubl++) * (*xubl++) + (*ubc++) * (*xubc++) + (*ubr++) * (*xubr++) +
          (*ucl++) * (*xucl++) + (*ucc++) * (*xucc++) + (*ucr++) * (*xucr++) +
          (*ufl++) * (*xufl++) + (*ufc++) * (*xufc++) + (*ufr++) * (*xufr++) ;

#if VALIDITY_CHECK == 1
        if(firstPass==1)
        {
          if(ii==imin)
          {
            if( fabs(*lhs-1.0) > 1.0e-5 )
            {
              printf("lhs[%d,%d,%d]=%e, should be 1.000000\n",ii,jj,kk,*lhs);
              abort();
            }
          }
          else
          {
            if( fabs(*lhs - 6.075507e-13) > 1.0e-5)
            {
              printf("lhs[%d,%d,%d]=%e, should be 6.075507e-13\n",ii,jj,kk,*lhs);
              abort();
            }
          }
        }
#endif /* end of VALIDITY_CHECK */
        lhs++;
        
      }/* end of ii loop */

      POINTER_INCREMENT( 2 );
          
    }/* end of jj loop */

    POINTER_INCREMENT( 2*jp );
    
  }/* end of kk loop */

#if VALIDITY_CHECK == 1
  if(firstPass==1)
    firstPass=0;
#endif  
  
#elif LOOP_IMPLEMENTATION == 4
  
  POINTER_INCREMENT( kmin*kp );
  
  for ( kk = kmin ; kk < kmax ; kk++ ) {
    
    POINTER_INCREMENT( jmin*jp );
    
    for ( jj = jmin ; jj < jmax ; jj++ ) {
      
      POINTER_INCREMENT( 2 );
      
      for ( ii = imin ; ii < imax ; ii++ ) {
        i = ii + jj * jp + kk * kp ;

        *lhs = (*dbl) * (*xdbl) + (*dbc) * (*xdbc) + (*dbr) * (*xdbr) +
          (*dcl) * (*xdcl) + (*dcc) * (*xdcc) + (*dcr) * (*xdcr) +
          (*dfl) * (*xdfl) + (*dfc) * (*xdfc) + (*dfr) * (*xdfr) +
          (*cbl) * (*xcbl) + (*cbc) * (*xcbc) + (*cbr) * (*xcbr) +
          (*ccl) * (*xccl) + (*ccc) * (*xccc) + (*ccr) * (*xccr) +
          (*cfl) * (*xcfl) + (*cfc) * (*xcfc) + (*cfr) * (*xcfr) +
          (*ubl) * (*xubl) + (*ubc) * (*xubc) + (*ubr) * (*xubr) +
          (*ucl) * (*xucl) + (*ucc) * (*xucc) + (*ucr) * (*xucr) +
          (*ufl) * (*xufl) + (*ufc) * (*xufc) + (*ufr) * (*xufr) ;

#if VALIDITY_CHECK == 1
        if(firstPass==1)
        {
          if(ii==imin)
          {
            if( fabs(*lhs-1.0) > 1.0e-5 )
            {
              printf("lhs[%d,%d,%d]=%e, should be 1.000000\n",ii,jj,kk,*lhs);
              abort();
            }
          }
          else
          {
            if( fabs(*lhs - 6.075507e-13) > 1.0e-5)
            {
              printf("lhs[%d,%d,%d]=%e, should be 6.075507e-13\n",ii,jj,kk,*lhs);
              abort();
            }
          }
        }
#endif /* end of VALIDITY_CHECK */
      
        POINTER_INCREMENT_ONE;
        
      }/* end of ii loop */

      POINTER_INCREMENT( 2 );
          
    }/* end of jj loop */

    POINTER_INCREMENT( 2*jp );
    
  }/* end of kk loop */

#if VALIDITY_CHECK == 1
  if(firstPass==1)
    firstPass=0;
#endif
  
#elif LOOP_IMPLEMENTATION == 5
  
  xdbl += kmin*kp;
  lhs += kmin*kp;
  COEFF_POINTER_INCREMENT( kmin*kp );
  
  for ( kk = kmin ; kk < kmax ; kk++ ) {
    
    xdbl += jmin*jp;
    lhs += jmin*jp;
    COEFF_POINTER_INCREMENT( jmin*jp );
    
    for ( jj = jmin ; jj < jmax ; jj++ ) {
      
      xdbl += 2;
      lhs += 2;
      COEFF_POINTER_INCREMENT( 2 );
      
      for ( ii = imin ; ii < imax ; ii++ ) {

        temp = (*dbl++) * (*xdbl++);
        temp += (*dbc++) * (*xdbl++);
        temp += (*dbr++) * (*xdbl);
        xdbl+=jp-2;
        temp += (*dcl++) * (*xdbl++);
        temp += (*dcc++) * (*xdbl++);
        temp += (*dcr++) * (*xdbl);
        xdbl += jp-2;
        temp += (*dfl++) * (*xdbl++);
        temp += (*dfc++) * (*xdbl++);
        temp += (*dfr++) * (*xdbl);
        xdbl += kp-2*jp-2;
        temp += (*cbl++) * (*xdbl++);
        temp += (*cbc++) * (*xdbl++);
        temp += (*cbr++) * (*xdbl);
        xdbl += jp-2;
        temp += (*ccl++) * (*xdbl++);
        temp += (*ccc++) * (*xdbl++);
        temp += (*ccr++) * (*xdbl);
        xdbl += jp-2;
        temp += (*cfl++) * (*xdbl++);
        temp += (*cfc++) * (*xdbl++);
        temp += (*cfr++) * (*xdbl);
        xdbl += kp-2*jp-2;
        temp += (*ubl++) * (*xdbl++); 
        temp += (*ubc++) * (*xdbl++);
        temp += (*ubr++) * (*xdbl);
        xdbl += jp-2;
        temp += (*ucl++) * (*xdbl++);
        temp += (*ucc++) * (*xdbl++);
        temp += (*ucr++) * (*xdbl);
        xdbl += jp-2;
        temp += (*ufl++) * (*xdbl++);
        temp += (*ufc++) * (*xdbl++);
        temp += (*ufr++) * (*xdbl) ;
        *lhs = temp;
        
#if VALIDITY_CHECK == 1
        if(firstPass==1)
        {
          if(ii==imin)
          {
            if( fabs(*lhs-1.0) > 1.0e-5 )
            {
              printf("at [%d,%d,%d], *lhs=%e, should be 1.000000\n",ii,jj,kk,*lhs);
              abort();
            }
          }
          else
          {
            if( fabs(*lhs - 6.075507e-13) > 1.0e-5)
            {
              printf("at [%d,%d,%d], *lhs=%e, should be 6.075507e-13\n",ii,jj,kk,*lhs);
              abort();
            }
          }
        }
#endif /* end of VALIDITY_CHECK */        

        lhs++;
        /* shift xdbl back to where it started from +1 */
        xdbl -= 2*kp + 2*jp + 1;
        
      }/* end of ii loop */
      
      xdbl += 2;
      lhs += 2;
      COEFF_POINTER_INCREMENT( 2 );
      
    }/* end of jj loop */

    xdbl += 2*jp;
    lhs += 2*jp;
    COEFF_POINTER_INCREMENT( 2*jp );
    
  }/* end of kk loop */

#if VALIDITY_CHECK == 1
  if(firstPass==1)
    firstPass=0;
#endif  
  
#elif LOOP_IMPLEMENTATION == 6
  
  for ( kk = kmin ; kk < kmax ; kk++ ) {
    for ( jj = jmin ; jj < jmax ; jj++ ) {
      for ( ii = imin ; ii < imax ; ii++ ) {
        i = ii + jj * jp + kk * kp ;

        b[i] =
          dbl[i] * (*(x+(ii-1+(jj-1)*jp+(kk-1)*kp))) +
          dbc[i] * (*(x+(ii+(jj-1)*jp+(kk-1)*kp))) +
          dbr[i] * (*(x+(ii+1+(jj-1)*jp+(kk-1)*kp))) +
          dcl[i] * (*(x+(ii-1+(jj)*jp+(kk-1)*kp))) +
          dcc[i] * (*(x+(ii+(jj)*jp+(kk-1)*kp))) +
          dcr[i] * (*(x+(ii+1+(jj)*jp+(kk-1)*kp))) +
          dfl[i] * (*(x+(ii-1+(jj+1)*jp+(kk-1)*kp))) +
          dfc[i] * (*(x+(ii+(jj+1)*jp+(kk-1)*kp))) +
          dfr[i] * (*(x+(ii+1+(jj+1)*jp+(kk-1)*kp))) +
          cbl[i] * (*(x+(ii-1+(jj-1)*jp+(kk)*kp))) +
          cbc[i] * (*(x+(ii+(jj-1)*jp+(kk)*kp))) +
          cbr[i] * (*(x+(ii+1+(jj-1)*jp+(kk)*kp))) +
          ccl[i] * (*(x+(ii-1+(jj)*jp+(kk)*kp))) +
          ccc[i] * (*(x+(ii+(jj)*jp+(kk)*kp))) +
          ccr[i] * (*(x+(ii+1+(jj)*jp+(kk)*kp))) +
          cfl[i] * (*(x+(ii-1+(jj+1)*jp+(kk)*kp))) +
          cfc[i] * (*(x+(ii+(jj+1)*jp+(kk)*kp))) +
          cfr[i] * (*(x+(ii+1+(jj+1)*jp+(kk)*kp))) +
          ubl[i] * (*(x+(ii-1+(jj-1)*jp+(kk+1)*kp))) +
          ubc[i] * (*(x+(ii+(jj-1)*jp+(kk+1)*kp))) +
          ubr[i] * (*(x+(ii+1+(jj-1)*jp+(kk+1)*kp))) +
          ucl[i] * (*(x+(ii-1+(jj)*jp+(kk+1)*kp))) +
          ucc[i] * (*(x+(ii+(jj)*jp+(kk+1)*kp))) +
          ucr[i] * (*(x+(ii+1+(jj)*jp+(kk+1)*kp))) +
          ufl[i] * (*(x+(ii-1+(jj+1)*jp+(kk+1)*kp))) +
          ufc[i] * (*(x+(ii+(jj+1)*jp+(kk+1)*kp))) +
          ufr[i] * (*(x+(ii+1+(jj+1)*jp+(kk+1)*kp))) ;

#if VALIDITY_CHECK == 1
        if(firstPass==1)
        {
          if(ii==imin)
          {
            if( fabs(b[i]-1.0) > 1.0e-5 )
            {
              printf("b[%d,%d,%d]=%e, should be 1.000000\n",ii,jj,kk,b[i]);
              abort();
            }
          }
          else
          {
            if( fabs(b[i] - 6.075507e-13) > 1.0e-5)
            {
              printf("b[%d,%d,%d]=%e, should be 6.075507e-13\n",ii,jj,kk,b[i]);
              abort();
            }
          }
        }
#endif /* end of VALIDITY_CHECK */
        
      }/* end of ii loop */
    }/* end of jj loop */
  }/* end of kk loop */
  
#if VALIDITY_CHECK == 1
  if(firstPass==1)
    firstPass=0;
#endif

    
#elif LOOP_IMPLEMENTATION == 7
  
  for ( kk = kmin ; kk < kmax ; kk++ ) {
    for ( jj = jmin ; jj < jmax ; jj++ ) {
      for ( ii = imin ; ii < imax ; ii++ ) {
        i = ii + jj * jp + kk * kp ;

        b[i] =
          dbl[i] * (*(x+(ii-1+(jj-1)*jp+(kk-1)*kp))) +
          dbc[i] * (*(x+(ii+(jj-1)*jp+(kk-1)*kp))) +
          dbr[i] * (*(x+(ii+1+(jj-1)*jp+(kk-1)*kp))) +
          dcl[i] * (*(x+(ii-1+(jj)*jp+(kk-1)*kp))) +
          dcc[i] * (*(x+(ii+(jj)*jp+(kk-1)*kp))) +
          dcr[i] * (*(x+(ii+1+(jj)*jp+(kk-1)*kp))) +
          dfl[i] * (*(x+(ii-1+(jj+1)*jp+(kk-1)*kp))) +
          dfc[i] * (*(x+(ii+(jj+1)*jp+(kk-1)*kp))) +
          dfr[i] * (*(x+(ii+1+(jj+1)*jp+(kk-1)*kp))) +
          cbl[i] * (*(x+(ii-1+(jj-1)*jp+(kk)*kp))) +
          cbc[i] * (*(x+(ii+(jj-1)*jp+(kk)*kp))) +
          cbr[i] * (*(x+(ii+1+(jj-1)*jp+(kk)*kp))) +
          ccl[i] * (*(x+(ii-1+(jj)*jp+(kk)*kp))) +
          ccc[i] * (*(x+(ii+(jj)*jp+(kk)*kp))) +
          ccr[i] * (*(x+(ii+1+(jj)*jp+(kk)*kp))) +
          cfl[i] * (*(x+(ii-1+(jj+1)*jp+(kk)*kp))) +
          cfc[i] * (*(x+(ii+(jj+1)*jp+(kk)*kp))) +
          cfr[i] * (*(x+(ii+1+(jj+1)*jp+(kk)*kp))) +
          ubl[i] * (*(x+(ii-1+(jj-1)*jp+(kk+1)*kp))) +
          ubc[i] * (*(x+(ii+(jj-1)*jp+(kk+1)*kp))) +
          ubr[i] * (*(x+(ii+1+(jj-1)*jp+(kk+1)*kp))) +
          ucl[i] * (*(x+(ii-1+(jj)*jp+(kk+1)*kp))) +
          ucc[i] * (*(x+(ii+(jj)*jp+(kk+1)*kp))) +
          ucr[i] * (*(x+(ii+1+(jj)*jp+(kk+1)*kp))) +
          ufl[i] * (*(x+(ii-1+(jj+1)*jp+(kk+1)*kp))) +
          ufc[i] * (*(x+(ii+(jj+1)*jp+(kk+1)*kp))) +
          ufr[i] * (*(x+(ii+1+(jj+1)*jp+(kk+1)*kp))) ;

#if VALIDITY_CHECK == 1
        if(firstPass==1)
        {
          if(ii==imin)
          {
            if( fabs(b[i]-1.0) > 1.0e-5 )
            {
              printf("b[%d,%d,%d]=%e, should be 1.000000\n",ii,jj,kk,b[i]);
              abort();
            }
          }
          else
          {
            if( fabs(b[i] - 6.075507e-13) > 1.0e-5)
            {
              printf("b[%d,%d,%d]=%e, should be 6.075507e-13\n",ii,jj,kk,b[i]);
              abort();
            }
          }
        }
#endif /* end of VALIDITY_CHECK */
        
      }/* end of ii loop */
    }/* end of jj loop */
  }/* end of kk loop */
  
#if VALIDITY_CHECK == 1
  if(firstPass==1)
    firstPass=0;
#endif
  
#endif
  
  myflops += 53.0 * ( imax - imin ) * ( jmax - jmin ) * ( kmax - kmin ) ;
    
}
