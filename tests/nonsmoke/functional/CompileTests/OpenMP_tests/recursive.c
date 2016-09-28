int counter = 0;
int foo()
{
#pragma omp critical
  {
  counter++;
  if (counter <100)
    foo();
 }
}

int main()
{
 foo();
  return 0;
}
