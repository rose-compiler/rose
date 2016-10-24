static double x[2];
extern void vranlc(double* y);
void foo()
{
  vranlc(x-1);
  vranlc(x+1);
}

