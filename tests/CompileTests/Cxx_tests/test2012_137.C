
template<class T>
class SwigValueWrapper
   {
     public:
       // C++ casting operator definition
          operator T&() const;
       // bool operator*(T & x);
   };

class X
   {
     operator int&() const;
  // bool operator*(X & x);
   };
   
// bool operator*(X & x,X & y);

void foo()
   {
  // Case using non-nested template class (any class type)
     SwigValueWrapper< int > X_result;
   }



