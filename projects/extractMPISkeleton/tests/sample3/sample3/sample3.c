int main( )
{
  int x=0;
  int i;

  #pragma  skel loop iterate exactly(12)
  for (i=0; x < 100 ; i++) {
    if (x == 7) continue;
    if (x % 2)
      x += 5;

  }

  return x;
}
