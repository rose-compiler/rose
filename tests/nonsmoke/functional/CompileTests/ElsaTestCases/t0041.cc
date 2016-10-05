// cc.in41
// operator[] overloading

class Foo {
public:
  int operator[] (int index);
};

int main()
{
  Foo f;
  return f[4];
}
