// cc.in14
// a few of the obscure expression kinds

// this would normally come from the <typeinfo> header
namespace std {
  class type_info {
  public:
    char const *name() const;
  };
}

typedef char y;

int main()
{
  int x, *p, *pp;

  // E_constructor
  x = int(6);

  // E_new
  p = new int;
  
  // E_new of an array with non-const size
  p = new int[x];

  // E_new of an array of an array; this allocates
  // an array of objects, where each object has type
  // "int[5]", and 'x' objects are allocated
  pp = new int[x][5];

  // E_delete
  delete p;

  // E_keywordCast
  x = const_cast<int>(x);
  x = dynamic_cast<int>(x);
  x = static_cast<int>(x);
  x = reinterpret_cast<int>(x);

  // E_typeidExpr
  typeid(x);
  
  // E_typeidType
  typeid(y);
}
