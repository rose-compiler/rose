// accumulation of  data members
//
//
//

typedef struct BLK {
  double * ccl; 
  double *cbr; 
  double *cbc;
  double *cbl; 
} Block_t;

void foo (int nnalls, Block_t* mblk, double dtn)
{
  for ( int i = 0 ; i < nnalls ; i++ ) {

    mblk->ccl[i] *= dtn ;
    mblk->cbr[i] *= dtn ;
    mblk->cbc[i] *= dtn ;
    mblk->cbl[i] *= dtn ;
  }
}
