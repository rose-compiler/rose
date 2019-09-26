template<class T, const T* i_>
void foo(T t);

struct X {};

extern const X x;

void foobar()
   {
     foo<X, &x>(x);
   }
