/*
When compiling the following code:
class Foo{
  public:
   Foo(int z);

};

class NullClass : public Foo{
 public:
   NullClass() : Foo(sizeof(Foo*)){};
   static void callWhenNull(int i){};

};

I get the following error:
rose_test.C: In constructor 'NullClass::NullClass()':
rose_test.C:13: error: types may not be defined in 'sizeof' expressions

This is due to incorrect unparsing of the Foo(sizeof(Foo*):

class Foo
{
 public: Foo(int z);
}

;

class NullClass : public Foo
{
 public: inline NullClass() : Foo(((sizeof(class Foo { public: Foo(int z);}*)))) {} inline static void callWhenNull(int i) {}
}; 
 */



class Foo
   {
     public:
          Foo(int z);
   };

class NullClass : public Foo
   {
     public:
          NullClass() : Foo(sizeof(Foo*)){};
          static void callWhenNull(int i){};
   };

