/* This is a subset of in12.c example */
typedef struct 
   {
     int nEGlo;
   } DelayedSpectrum;

void foobar()
   {
  /* This should translate to: "DelayedSpectrum *Spec = 0;" */
     shared DelayedSpectrum *Spec = 0;
   }
