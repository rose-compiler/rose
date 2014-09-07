// 8 Matches

//adapted from
//http://en.wikipedia.org/wiki/Comma_operator

int main() {
  int a=1, b=2, c=3, i=0;
  i = (a, b);
  i = a, b;
  i = (a += 2, a + b);
  i = a += 2, a + b;
  i = a, b, c;
  i = (a, b, c);
  return 0;
}
