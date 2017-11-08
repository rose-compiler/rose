typedef struct BLK 
{

  double * ccl; 
  double * cbr; 
  double * cbc; 
  double * cbl; 
}
Block_t;

void foo(int nnalls, double dtn, Block_t * rblk)
{
  double* ccl = rblk->ccl;
  double* cbr = rblk->cbr;
  double* cbc = rblk->cbc;
  double* cbl = rblk->cbl;

  for (int i = 0; i < nnalls ; i++) {

    ccl[i] *= dtn ;
    cbr[i] *= dtn ;
    cbc[i] *= dtn ;
    cbl[i] *= dtn ;

  }
}
