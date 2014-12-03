//255
namespace X {
    class A{};
}

namespace Y {
    class A{};
}

namespace X {
    Y::A a;
}

namespace Y {
    X::A b;
}
