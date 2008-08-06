template<typename t>
class Foo
{
  public:
    Foo(){}
    ~Foo(){}
};

int main()
{
  Foo<int> fi;
  Foo<float> ff;
  return 0;
}
