/* This is a copy of in15.c */
#ifdef NOTDEF
#define SHARED /**/
#else
#define SHARED shared
#endif

typedef struct ONEeIn_s {
   SHARED double *xOutEP;
} ONEeIn;

typedef struct {
   SHARED ONEeIn *EPTabl;
} EProb; 

typedef struct {
   EProb EP;
} DelayedSpectrum;

void foobar(SHARED DelayedSpectrum *Spec)
{
	double foo;

	foo = *((Spec->EP).EPTabl->xOutEP);
	/* foo =  *((double *)(((char *)(((ONEeIn *)(((char *)((DelayedSpectrum *)(((char *)Spec) + MPISMOFFSET)) -> EP . EPTabl) + MPISMOFFSET)) -> xOutEP)) + MPISMOFFSET)); */
}
