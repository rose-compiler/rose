int main( )
{
  int x=0;
  int i,j;

  #pragma skel loop iterate atleast(10)
  do {
    x = i + 1;
    if (x % 2)
      x += 5;
    i++;
  } while (x < 100);

  #pragma skel loop iterate atmost(30)
  while (x != 512)
    x += 11;

  #pragma skel loop iterate exactly(10*10)
  while (x != 512)
    x += 11;

  return x;
}
