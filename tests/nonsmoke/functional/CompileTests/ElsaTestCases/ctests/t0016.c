// t0016.c
// function prototype and definition with different parameter lists

int fallowc(int);

fallowc(c)
char c;        // "default argument promotions" make this int
{
  return 0;
}
