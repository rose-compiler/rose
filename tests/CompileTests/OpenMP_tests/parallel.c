int main(void)
{
  int i, a[1000];
#pragma omp parallel if(i<100)
    a[i]=i*2;
return 1;
}
