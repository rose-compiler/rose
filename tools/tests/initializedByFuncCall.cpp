// a function call is used to initialize each variable
// Not the form of   = base+ offset; 

class Domain_t; 

extern double* MeshVariable_getDoublePtr(const char* var_name,
                                         struct Domain_t* domain);

void foo( Domain_t *domain, int length, const int* ndx, 
        double * ratomFrac, double* rmassFrac, double rabar, 
        double rzbar, double cclog)
{
  double *amass   = MeshVariable_getDoublePtr( "l_amass",  domain );
  double *anum    = MeshVariable_getDoublePtr( "l_anum",   domain );
  double *iden    = MeshVariable_getDoublePtr( "l_iden",   domain );
  double *coulog  = MeshVariable_getDoublePtr( "l_loglam",   domain );

  for ( int i = 0; i < length; i++ ) {

    const int ndxidx = ndx[i];
    amass [ndxidx] += ratomFrac[i] * rabar;
    anum  [ndxidx] += ratomFrac[i] * rzbar;
    iden  [ndxidx] += rmassFrac[i] / rabar;
    coulog[ndxidx] += rmassFrac[i] * cclog;
  } 
}
