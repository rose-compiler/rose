

void foo(int *x)
{
  int local;
  *x = 2;
  local = 2;
}

int main(int argc, char **argv)
{
  int y;
  foo(&y);
}
