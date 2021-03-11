bool foobar()
{
  int* abvar;
#pragma rose_outline
  if (abvar != 0L)
  {
    bool rose_result = false;
    return rose_result;
  }     
  return false;
}
