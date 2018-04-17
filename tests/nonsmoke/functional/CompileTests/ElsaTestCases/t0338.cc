// t0338.cc
// access a type_info

// this would normally come from the <typeinfo> header
namespace std {
  class type_info {
  public:
    char const *name() const;
  };
}

void foo()
{
  int i;
  
  typeid(i).name();
}
