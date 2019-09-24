namespace Namespace_1 {}

namespace Namespace_2 {
  class Class_1 {};
}

namespace Namespace_1 {
  template< typename T>
  class Class_2 {};

  template class Class_2<Namespace_2::Class_1>;
}
