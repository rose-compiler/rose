// A test with pointers without sharing a common base address.
extern double* GetDoublePtr(const char* var_name);
                                   
static void ASCLaserAPI_fill_setup_meshvars(  const int * rndx[], int namix, int nreg, 
    int numGroups, double *pa , int a , int b, int c, int d, int *rlen, int *rlenmix)
{
  // declared in a sequence of statements
  double *amass = GetDoublePtr ("l_amass" );
  double *anum = GetDoublePtr ("l_anum" );
  double *zstar2 = GetDoublePtr ("l_zstar2" );
  double *iden = GetDoublePtr ("l_iden" );
  double *coulog = GetDoublePtr ("l_loglam");

  double *amassmix = pa; 

  for (int ir = 1; ir <= nreg; ir++)
  {

    const int *ndx = rndx[ir];
    int length = rlen[ir];
    int mixlen = rlenmix[ir];

    for (int igrp = 0; igrp < numGroups; igrp++)
    {
      // False positive here: 
      for (int i = 0; i < length; i++)
      {
        const int ndxidx = ndx[i];
        amass[ndxidx] += a; 
        anum[ndxidx] += b; 
        iden[ndxidx] += c; 
        coulog[ndxidx] += d;
      }
    }
  }
}

