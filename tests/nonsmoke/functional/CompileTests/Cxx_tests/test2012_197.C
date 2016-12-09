int main()
{
  const int size = 5;
  int array2 [1 + size]; // `size` constant is propagated
  return 0;
}
