namespace N { class A; }
void bar (const N::A&);

namespace N
{
  // This 'bar' is not the true friend.
  void bar (const A&);
  class A
  {
  public:
    A () {
  ::bar (*this);  // name qualifier is essential here
   }
    friend void ::bar (const A&); // '::' must be preserved.
  };
}
