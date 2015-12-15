template <typename T>
class X
   {
     public:
          friend void foo( X<T> & i )
             {
            // int abc = 7;
            // abc++;
             }
   };

// Because the member function (or friend function) declaration will not be output in the template string used for unparsing,
// we need to output:  "template<typename T> inline void foo(class X< int  > &i) {}"
// the friend or member function must not be output as a template specialization.

#if 1
void foobar()
   {
     X<int> x;

  // Error if this is output as: "::foo(x);"
     foo(x);
   }
#endif
