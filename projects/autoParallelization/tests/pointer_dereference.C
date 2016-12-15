
#define max_ir 100

int * nd_array[max_ir];
int * gr_array[max_ir];

void foo (int rlenmix)
{
  int i;
  for ( i = 0 ; i < rlenmix ; i++ ) {
    int nn = *(nd_array[1] + i);
    int gg = *(gr_array[1] + i);

    nn = *(nd_array[2] + i);
    gg = *(gr_array[2] + i);
  }
}

