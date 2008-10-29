int counter = 0;
int foo()
{
#pragma omp critical
  {
  counter++;
 }
return 0; 
}

int main()
{
 foo();
  return 0;
}
