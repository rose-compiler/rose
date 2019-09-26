int a_[4];

class c_
   {
     public:
          auto f()const -> int(*)[4] { return &a_; }
   };

