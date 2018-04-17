// void may_throw();
// void no_throw() noexcept;

auto lmay_throw = []{};
auto lno_throw = []() noexcept {};

void foobar()
   {
     noexcept(lmay_throw());
     noexcept(lno_throw());
   }
