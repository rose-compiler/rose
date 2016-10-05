typedef struct {
} EProb; 

typedef struct {
   EProb EP;
} DelayedSpectrum;

void foobar(shared DelayedSpectrum *Spec)
{
	EProb lEProb;
/* lEProb = ((DelayedSpectrum *)(((char *)Spec) + MPISMOFFSET)) -> EP; */
	lEProb = Spec->EP;

}
