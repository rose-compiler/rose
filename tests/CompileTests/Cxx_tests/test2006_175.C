class Foo
{
public:
    Foo(const Foo &right);
    virtual  const Foo & operator=(const Foo &right);
};

Foo::Foo(const Foo &Right)
{
    *this=Right;
}
