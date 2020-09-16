/*
 * Test case for declaration movement
 *  
 * */
int x;
int y;
extern int f(int );
extern int foo(int );
extern int goo(int );
extern int g(int );

void func1(int len)
{
/* declared once, used multiple times as loop index variable*/
/* declared once, used multiple times */
  for (int i = 0; i < len; ++i) {
    int tmp = f(i);
    x = foo(tmp);
/* … */
  }
  for (int i = 0; i < len; ++i) {
    int tmp = g(i);
    y = goo(tmp);
/* … */
  }
}

void func2(int len)
{
/* declared once, used multiple times as loop index variable*/
/* declared once, used multiple times */
{
    int tmp;
    for (int i = 0; i < len; ++i) {
      tmp = f(i);
      x = foo(tmp);
/* … */
    }
    for (int i = 0; i < len; ++i) {
      tmp = g(i) + tmp;
// here is live in!
      y = goo(tmp);
/* … */
    }
  }
}
