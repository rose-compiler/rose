// This is a copy of test2005_150.C (with some modifications).

// Debug ambiguous template specialization if template <> X :: X() {}

template <typename T>
class X
   {
     public:
          X(T t) {};
   };

int main()
   {
     X<int> a(1);
     X<int> b(1.0);
   }

