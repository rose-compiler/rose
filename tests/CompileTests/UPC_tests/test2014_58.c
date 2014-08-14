/* This is a subset of the in12.c example */
typedef struct {} EProb; 

typedef struct 
   {
     EProb EP;
   } DelayedSpectrum;

// void myfunc(shared EProb * input_EP);
// void myfunc_2(shared DelayedSpectrum * input_DS);

// shared DelayedSpectrum *Spec = 0L;

DelayedSpectrum XXX;

void foobar()
   {
     EProb localEP = XXX.EP;

//   myfunc(&(Spec->EP));
//   myfunc_2(Spec);
 /* Translates to myfunc(&(Spec->EP)); because myfunc takes a shared address to EP. */
}
