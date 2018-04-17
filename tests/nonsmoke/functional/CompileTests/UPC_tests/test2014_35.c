/* This is the in12.c example */
typedef struct ONEeIn_s {
   double eIn;
   shared double *xOutEP;
} ONEeIn;

typedef struct {
   int type;
   int   LeInTabl;
   int   LxOutEP;
   shared ONEeIn *EPTabl;
} EProb; 

typedef struct {
   double tau;
   int nEGlo;
   int nEGhi;
   shared double *xNu;
   EProb EP;
} DelayedSpectrum;

void foobar()
{
	shared DelayedSpectrum *Spec = 0;
	void myfunc(shared EProb * EP);
	myfunc(&(Spec->EP));
 /* Translates to myfunc(&(Spec->EP)); because myfunc takes a shared address to EP. */
}
