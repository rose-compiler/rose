/* This is a subset of the in12.c example */
typedef struct {} EProb; 

typedef struct 
   {
     EProb EP;
   } DelayedSpectrum;

void myfunc(shared EProb * input_EP);
// void myfunc_2(shared DelayedSpectrum * input_DS);

void foobar()
   {
     shared DelayedSpectrum *Spec = 0;

     shared EProb* eprob_var = &(Spec->EP);

//   myfunc(&(Spec->EP));
//   myfunc_2(Spec);
 /* Translates to myfunc(&(Spec->EP)); because myfunc takes a shared address to EP. */
}
