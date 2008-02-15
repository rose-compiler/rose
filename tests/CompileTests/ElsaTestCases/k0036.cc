// taking the address of a string (?!)

// originally found in package psys

// error: cannot take address of non-lvalue `char [5]'

// ERR-MATCH: address of non-lvalue `char

void f1(int *);     
void g1(int [5]);
void f2(int (*)[5]);

void f3(char const *);
void f4(char const (*)[5]);

template <class T>
void f(T);

template <class T>
void g(T (&)[5]);

int main()
{
  int a[5];
  &a;

  f1(a);
  g1(a);
  f2(&a);

  //ERROR(1): f1(&a);
  //ERROR(2): f2(a);

  f3("blah");
  f4(&"blah");

  //ERROR(3): f3(&"blah");
  //ERROR(4): f4("blah");

  f(a);
  f(&a);
  f("blah");
  f(&"blah");
  
  g(a);
  g("blah");
}
