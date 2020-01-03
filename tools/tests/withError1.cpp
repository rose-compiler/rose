// A test with an error in purpose, to test keep_going option, avoid infinite looping
void foo(ddouble* v, int jp, double* volave, int numRealZones, int* Zones, double* vol)
{
  double *v1;
  double *v2;
  double *v3;
  double *v4 = volave; // v4 is first defined here

  v4 = v ;   // v4 is defined again here.
  v1 = v4 + 1 ; 
  v2 = v1 + jp ; 
  v3 = v4 + jp ;

  for ( int ii = 0; ii < numRealZones; ii++) {

    int i = Zones[ii];

    double dv = 0.25 * vol[i];

    v1[i] += dv ;
    v2[i] += dv ;
    v3[i] += dv ;
    v4[i] += dv ;

  }
}
