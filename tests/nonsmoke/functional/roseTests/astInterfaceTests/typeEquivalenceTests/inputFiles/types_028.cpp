//255
namespace {
  class A{};
}
namespace X{
  class A{};
  namespace Z{
    class A{};
    namespace Y{
      class A{};
    }
  }
  Z::Y::A b;
}
A a;

