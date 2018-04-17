int main()
{
  int t, i, j, Ca, b, N, M;
#pragma scop
  for (t=0; t<N; t++) {
    for (i=0; i<M; i++) 
      for (j=0; j<N; j++)
	Ca = 0;
    for (j=0; j<N; j++)  
      b = 0;
  }
#pragma endscop
}
