// Input file to test mangling of SgFunctionDeclaration objects.


long foobar();
long foobar(int);
long foobar(int y);
long foobar(int x);
long foobar(int x = 0);
long foobar(int xyz) 
   {
     return xyz;
   }

char foobarChar(char);
char foobarChar(char c);

// Input file to test mangling of SgFunctionDeclaration objects.

typedef int value0_t;
typedef value0_t value_t;
namespace N
   {
     typedef struct { int a; } s_t;
     class A { public: A () {} virtual void foo (int) {} };
     class B { public: B () {} void foo (value_t) const {} };
     class C : public A { public: C () {} void foo (int) {} void foo (const s_t&) {} };
     void foo (const s_t*) {}
   }

typedef N::s_t s2_t;
void foo (value_t);
void foo (s2_t) {}
void foo (float x[]) {}
void foo (value_t, s2_t);

template <typename T>
void foo (T) {}

namespace P
   {
     typedef long double type_t;
     namespace Q
        {
          template <typename T>
          void foo (T) {}

          class R
             {
               public:
                    R () {}
                    template <typename T>
                    void foo (T) {}
                    void foo (P::type_t) {}
                    template <typename T, int x>
                    int foo (T) { return x; }
             };
        }
   }

template <typename T, int x>
int foo (T) { return x; }

template void foo<char> (char);
template void foo<const value_t *> (const value_t *);
template void P::Q::foo<long> (long);
template void P::Q::R::foo<value_t> (value_t);




