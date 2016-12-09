// cc.in71
// duplicate typedef at class scope

class Foo {
public:
  typedef int x;
  typedef int x;
  
  typedef enum E { a,b,c } E;
};
