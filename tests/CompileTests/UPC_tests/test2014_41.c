/* This is a copy of in15.c */
#ifdef NOTDEF
#define SHARED /**/
#else
#define SHARED shared
#endif

typedef struct ONEeIn_s {
   double eIn;
   SHARED double *xOutEP;
} ONEeIn;

typedef struct {
   int type;
   int   LeInTabl;
   int   LxOutEP;
   SHARED ONEeIn *EPTabl;
} EProb; 

typedef struct {
   double tau;
   int nEGlo;
   int nEGhi;
   SHARED double *xNu;
   EProb EP;
} DelayedSpectrum;

void foobar(SHARED DelayedSpectrum *Spec)
{
	SHARED double *lxNu;
	EProb lEProb;
	SHARED ONEeIn *lEPTabl;
	SHARED double *lxOutEP;
	double foo;

	lxNu = Spec->xNu;
	/* lxNu = ((DelayedSpectrum *)(((char *)Spec) + MPISMOFFSET)) -> xNu; */
	lEProb = Spec->EP;
	/* lEProb = ((DelayedSpectrum *)(((char *)Spec) + MPISMOFFSET)) -> EP; */
	lEPTabl = (Spec->EP).EPTabl;
	/* lEPTabl = ((DelayedSpectrum *)(((char *)Spec) + MPISMOFFSET)) -> EP . EPTabl; */
	lxOutEP = (Spec->EP).EPTabl->xOutEP;
	/* lxOutEP = ((ONEeIn *)(((char *)((DelayedSpectrum *)(((char *)Spec) + MPISMOFFSET)) -> EP . EPTabl) + MPISMOFFSET)) -> xOutEP; */
	foo = *((Spec->EP).EPTabl->xOutEP);
	/* foo =  *((double *)(((char *)(((ONEeIn *)(((char *)((DelayedSpectrum *)(((char *)Spec) + MPISMOFFSET)) -> EP . EPTabl) + MPISMOFFSET)) -> xOutEP)) + MPISMOFFSET)); */
}
