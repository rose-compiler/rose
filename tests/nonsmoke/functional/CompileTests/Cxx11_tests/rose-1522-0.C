namespace std
{
  template<typename _Tp>
  class shared_ptr {};
}

namespace nsp {

class A;

class B {
  public:
    void foo(std::shared_ptr<A> && a);
};

void B::foo(std::shared_ptr<A> && a) {}

}
