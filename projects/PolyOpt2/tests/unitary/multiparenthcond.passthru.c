
int main()
{
  int M;
  int N;
  int a;
  
#pragma scop
  if (M > N + 1) 
    a;
  
#pragma endscop
}
