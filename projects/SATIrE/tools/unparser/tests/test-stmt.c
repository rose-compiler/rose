extern bar(int *a, int *b);
int foo() {
  int a = 5, b = 6;

  bar(&a, &b);

x: ;
  int c = a + b;
  int d = a - b;
  int e = --a;
  int f = a++;
  if (f)
    goto x;

  if (e)
    goto x;
  else
    goto x;

  switch(d) {
  case 1: goto x;
  case 2: d++;
  case 3:
    break;
  default:
    d--;
  }  

  return sizeof d + sizeof(int);
}