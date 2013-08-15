int main( )
{
  int x=0;
  int i,k;

  for (i=0; x < 100 ; i++) {
    x = i + 1;
    if (x % 2)
      x += 5;
    int j;
    for (j=0; x < 100 ; j++) {
      x = j + 1;
    }
  }

  for (k=0; x < 500 ; k++) {
    x = k + 1;
    if (x % 2)
      x += 5;
  }
  return x;
}
