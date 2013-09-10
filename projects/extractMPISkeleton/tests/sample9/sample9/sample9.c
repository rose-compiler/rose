int main( )
{
  int x = 5;
  int y = 0;

#pragma skel condition prob(6/10)  // make true 60% of time.
  if (x % 2) {
    x += 5;
  } else {
    x = 0;
  }

#pragma skel condition prob(4/10)
  if (x % 2) {
    x += 5;
  } else {

    #pragma skel condition prob(1/10)
    if (x) {
      x++;
    }
  }

  return x;
}
