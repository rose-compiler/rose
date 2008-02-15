// cc.in60
// invoke via operator()

class Foo {
public:
  int operator() ();
};

int main()
{
  Foo f;
  return f();
}
