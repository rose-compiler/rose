//int logics[1000];
//another regular array types, not from a parameter list
int main()
{
  int logic_or = 0;
  int i;

  int logics[1000];
#pragma omp parallel
  {
#pragma omp for schedule(dynamic,1)
    for (i = 0; i < 1000; ++i)
    {
      logic_or = logic_or || logics[i];
    }
  }
  return 0;
}

