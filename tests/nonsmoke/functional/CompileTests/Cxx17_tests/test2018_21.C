// Constexpr Lambda

auto monoid = [](auto v) { return [=] { return v; }; };
auto add = [](auto m1) constexpr {
     auto ret = m1();
     return [=](auto m2) mutable {
          auto m1val = m1();
          auto plus = [=] (auto m2val) mutable constexpr
                { return m1val += m2val; };
ret = plus(m2());
return monoid(ret);
    };
  };

  constexpr auto zero = monoid(0);
  constexpr auto one = monoid(1);

  static_assert(add(one)(zero)() == one());  // OK

// Since 'two' below is not declared constexpr, an evaluation of its constexpr member function call operator
// can not perform an lvalue-to-rvalue conversion on one of its subobjects (that represents its capture)
// in a constant expression.

  auto two = monoid(2);
  assert(two() == 2);

// OK, not a constant expression.
  static_assert(add(one)(one)() == two());

// ill-formed: two() is not a constant expression
  static_assert(add(one)(one)() == monoid(2)()); // OK

