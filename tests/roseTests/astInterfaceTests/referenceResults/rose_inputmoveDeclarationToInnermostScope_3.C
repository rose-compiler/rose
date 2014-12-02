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
  for (
/* declared once, used multiple times as loop index variable*/
int i = 0; i < len; ++i) {
/* declared once, used multiple times */
    int tmp;
    tmp = f(i);
    x = foo(tmp);
/* … */
  }
  for (
/* declared once, used multiple times as loop index variable*/
int i = 0; i < len; ++i) {
/* declared once, used multiple times */
    int tmp;
    tmp = g(i);
    y = goo(tmp);
/* … */
  }
}

void func2(int len)
{
{
/* declared once, used multiple times */
    int tmp;
    for (
/* declared once, used multiple times as loop index variable*/
int i = 0; i < len; ++i) {
      tmp = f(i);
      x = foo(tmp);
/* … */
    }
    for (
/* declared once, used multiple times as loop index variable*/
int i = 0; i < len; ++i) {
// here is live in!
      tmp = g(i) + tmp;
      y = goo(tmp);
/* … */
    }
  }
}
