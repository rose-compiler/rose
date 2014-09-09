/*
 * Test case for declaration movement
 *  
 * */
int x, y; 
extern int f(int );
extern int foo(int );
extern int goo(int );
extern int g(int );

void func1(int len)
{
  int i; /* declared once, used multiple times as loop index variable*/ 
  int tmp ; /* declared once, used multiple times */
  for (i=0; i<len; ++i) {
    tmp = f(i) ;
    x = foo(tmp) ;
    /* … */
  }

  for (i=0; i<len; ++i) {
    tmp = g(i) ;
    y = goo(tmp) ;

    /* … */
  }
}
