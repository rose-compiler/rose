
#define fmin(a,b) ((a) < (b)) ? (a) : (b)
#define fvswap(zzp1, zzp2, zzn)       \
{                                     \
   Int32 yyp1 = (zzp1);               \
   Int32 yyp2 = (zzp2);               \
   Int32 yyn  = (zzn);                \
   while (yyn > 0) {                  \
      yyp1++; yyp2++; yyn--;          \
   }                                  \
}

typedef char            Char;
typedef unsigned char   Bool;
typedef unsigned char   UChar;
typedef int             Int32;
typedef unsigned int    UInt32;
typedef short           Int16;
typedef unsigned short  UInt16;

typedef unsigned long long UInt64;
#define BZ_N_RADIX 2
#define FALLBACK_QSORT_SMALL_THRESH 10
#define FALLBACK_QSORT_STACK_SIZE   100

void mainQSort3 ( UInt32* ptr,
                  UChar*  block,
                  UInt16* quadrant,
                  Int32   nblock,
                  Int32   loSt,
                  Int32   hiSt,
                  Int32   dSt,
                  Int32*  budget );

void fallbackSimpleSort ( UInt32* fmap,
                          UInt32* eclass,
                          Int32   lo,
                          Int32   hi );

static
void fallbackQSort3 ( UInt32* fmap,
                      UInt32* eclass,
                      Int32   loSt,
                      Int32   hiSt )
{
  Int32 unLo, unHi, ltLo, gtHi, n, m;
  Int32 sp, lo, hi;
  UInt32 med, r, r3;
  Int32 stackLo[FALLBACK_QSORT_STACK_SIZE];
  Int32 stackHi[FALLBACK_QSORT_STACK_SIZE];

  r = 0;

  sp = 0;

  while (sp > 0) {

    if (hi - lo < FALLBACK_QSORT_SMALL_THRESH) {
      fallbackSimpleSort ( fmap, eclass, lo, hi );
      continue;
    }

    /* Random partitioning.  Median of 3 sometimes fails to
       avoid bad cases.  Median of 9 seems to help but 
       looks rather expensive.  This too seems to work but
       is cheaper.  Guidance for the magic constants 
       7621 and 32768 is taken from Sedgewick's algorithms
       book, chapter 35.
       */
    r = ((r * 7621) + 1) % 32768;
    r3 = r % 3;
    if (r3 == 0) med = eclass[fmap[lo]]; else
      if (r3 == 1) med = eclass[fmap[(lo+hi)>>1]]; else
        med = eclass[fmap[hi]];

    unLo = ltLo = lo;
    unHi = gtHi = hi;

    while (1) {
      while (1) {
        if (unLo > unHi) break;
        n = (Int32)eclass[fmap[unLo]] - (Int32)med;
        if (n == 0) {
          ltLo++; unLo++;
          continue;
        };
        if (n > 0) break;
        unLo++;
      }
      while (1) {
        if (unLo > unHi) break;
        n = (Int32)eclass[fmap[unHi]] - (Int32)med;
        if (n == 0) {
          gtHi--; unHi--;
          continue;
        };
        if (n < 0) break;
        unHi--;
      }
      if (unLo > unHi) break;
      unLo++; unHi--;
    }


    if (gtHi < ltLo) continue;

    n = fmin(ltLo-lo, unLo-ltLo); fvswap(lo, unLo-n, n);
    m = fmin(hi-gtHi, gtHi-unHi); fvswap(unLo, hi-m+1, m);

    n = lo + unLo - ltLo - 1;
    m = hi - (gtHi - unHi) + 1;

    if (n - lo > hi - m) {
    } else {
    }
  }

  // made up another test for if (condition)
  {
   UInt32* ptr;
   UChar*  block;
   UInt16* quadrant;
   Int32   nblock;
   Int32*  budget;
    Int32 lo ;
    Int32 hi ;
    if (hi > lo) {
      mainQSort3 (
          ptr, block, quadrant, nblock,
          lo, hi, BZ_N_RADIX, budget
          );
    }
  }

  // another if-stmt case
  // variable is used in both true and false body.
  // Naive analysis will find if-stmt is the inner-most common scope.
  { 
    int blockx;
    if (loSt)
      blockx ++;
    else
      blockx --;
   }

   //switch
   {
     int blockx; 
     int ttt;
     switch (blockx)
     {
       case 0:
         ttt ++;
         break;
       case 1:
         ttt --;
         break;
      }
   }
}

Int32 uInt64_qrm10 ( UInt64* n );
Bool uInt64_isZero ( UInt64* n );

void uInt64_toAscii ( char* outbuf, UInt64* n )
{
   Int32  i, q;
   UChar  buf[32];
   Int32  nBuf   = 0;
   UInt64 n_copy = *n;
   do {
      q = uInt64_qrm10 ( &n_copy );
      buf[nBuf] = q + '0';
      nBuf++;
   } while (!uInt64_isZero(&n_copy));
   outbuf[nBuf] = 0;
   for (i = 0; i < nBuf; i++)
      outbuf[i] = buf[nBuf-i-1];
}



