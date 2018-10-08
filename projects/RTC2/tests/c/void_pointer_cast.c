
void fn2(void* voidin)
{
  return 0;
}

void fn1(void* input)
{
  fn2((void*)input);
}

int main()
{
  fn2(0);
}
