struct X_
   {
     int i, j;
     static int ctor;
     X_(int ii = 32) : i(ii), j(++ctor) { }
     X_(int ii, int jj) : i(ii), j(jj) { ++ctor; }
   };

int X_::ctor = 0;

X_ a_[5] = { 1, X_(), X_(-2, 3), X_(), 32 };

// BUG: unparsed as: struct X_ b_[5] = {(1), X_(), X_(- 2,8), ()};
X_ b_[5] = { 1, X_(), X_(-2, 8) };


