
class Y {};

template < class T >
class X // : public X<float>
   {
     public:
          typedef X<T> type;

          type* var;
   };

void foo()
   {
     X<int> var2;
   }
