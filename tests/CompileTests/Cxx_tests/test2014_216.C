// Example of co-variant return type

// Just create a class, and a subclass
class Foo {};
class Bar : public Foo {};


class Baz 
{
public:
  virtual Foo * create()
    {
      return new Foo();
    }
};

class Quux : public Baz 
{
public:
  // Different return type, but it's allowed by the standard since Bar is derived from Foo
  virtual Bar * create()
    {
      return new Bar();
    }
};

  

int main() 
{
  Quux *tmp = new Quux();
  Bar *bar = tmp->create();
}
