// cc.in33
// calling constructors

class Foo {
public:
  Foo(int x);
};

typedef class Foo FILE;

int main()
{
  Foo f(3);

  int x,y;
  Foo g(x*y);

  int exit(int);

  // this one looks ambiguous at first blush
  int fileno(FILE *f);
}
