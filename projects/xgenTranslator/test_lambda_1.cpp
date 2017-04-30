

const int n =15;
double* a;  

// --------------------------------
void foo_input ()
{
  for (int ii= 0; ii< n; ++ii) { 
    [=](int i) {a[i] = 0.5;} (ii);
  }
}

// Inlining the lambda expression
void foo_output ()
{
  for (int ii = 0; ii < n; ++ii) 
  {
    a[ii] = 0.5;  
  }
}

int main()
{
  a = new double [n];
  foo_input();
  foo_output();

  free(a);
  return 0;
}
