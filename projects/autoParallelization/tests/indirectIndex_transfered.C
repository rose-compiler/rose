// indirect array access: naive form
void foo1(int* indexSet, int N, int ax)
{
  double *xa3 = new double[N]; 
  for (int idx =0; idx <N; ++idx) 
  {
    xa3[indexSet[idx]]+=ax;
    xa3[indexSet[idx]]+=ax;
  }
}

// indirect array access: transferred form
void foo2(int* indexSet, int N, int ax)
{
 double *xa3 = new double[N]; 
 for (int idx =0; idx <N; ++idx) 
 {
   const int i = indexSet[idx];
   xa3[i]+=ax;
   xa3[i]+=ax;
 }
  
  
}
