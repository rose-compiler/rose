int a_[4];

class c_
   {
          int i, j, k;
     public:
          auto f()const -> int(*)[4] { return &a_; }
          c_() : i(8), j(44) , k(844) { }
   };

const c_ co_;
