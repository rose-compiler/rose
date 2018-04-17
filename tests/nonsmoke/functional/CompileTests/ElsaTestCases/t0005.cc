// cc.in5
// some tricky ambiguous syntax examples

enum MyEnum { e8, e4=4, e5 };

int f(int, int);
int a, b, c, y, p;
void x(int);
typedef int q;

int main()
{
  +f(a,2);                // unambiguous call

  f( (a)&(b), c );        // ambiguous call

  {
    x(y);
    3;
  }

  (q)(p);
  
  3+4*5;
}
