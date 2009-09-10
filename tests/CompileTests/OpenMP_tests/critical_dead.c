int counter = 0;
void foo()
{
#pragma omp critical
  {
if (counter <100)
   counter++;
//else 
//  return;
//    foo();
 }
  foo();
}

int main()
{
 foo();
  return 0;
}

