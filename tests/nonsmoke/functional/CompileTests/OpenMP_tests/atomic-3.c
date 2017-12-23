
int foo (int x)
{
  int v; 
#pragma omp atomic read
     v= x;
#pragma omp atomic capture
     v= x--;
  return v;
}

int main (void)
{

  int a, b, c;
#pragma omp parallel 
  {
#pragma omp atomic write
    a=1;
#pragma omp atomic update 
    a++;
  }
  return 0;
}


