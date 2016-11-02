auto lmay_throw = []{};

void foobar()
   {
     noexcept(lmay_throw());
   }
