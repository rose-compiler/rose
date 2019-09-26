#include <new>
#include <cstddef>
#include <cassert>
 
struct X {
  const int n; // note: X has a const member
  int m;
};
 
struct Y {
  int z;
};
 
struct A { 
    virtual int transmogrify();
};
 
struct B : A {
    int transmogrify() override { new(this) A; return 2; }
};
 
int A::transmogrify() { new(this) B; return 1; }
 
static_assert(sizeof(B) == sizeof(A));
 
int main()
{
  X *p = new X{3, 4};
  const int a = p->n;
  X* np = new (p) X{5, 6};    // p does not point to new object because X::n is const; np does
  const int b = p->n; // undefined behavior
  const int c = p->m; // undefined behavior (even though m is non-const, p can't be used)
  const int d = std::launder(p)->n; // OK, std::launder(p) points to new object
  const int e = np->n; // OK
 
  alignas(Y) std::byte s[sizeof(Y)];
  Y* q = new(&s) Y{2};
  const int f = reinterpret_cast<Y*>(&s)->z; // Class member access is undefined behavior:
                                             // reinterpret_cast<Y*>(&s) has value "pointer to s"
                                             // and does not point to a Y object 
  const int g = q->z; // OK
  const int h = std::launder(reinterpret_cast<Y*>(&s))->z; // OK
 
  A i;
  int n = i.transmogrify();
  // int m = i.transmogrify(); // undefined behavior
  int m = std::launder(&i)->transmogrify(); // OK
  assert(m + n == 3);
}


