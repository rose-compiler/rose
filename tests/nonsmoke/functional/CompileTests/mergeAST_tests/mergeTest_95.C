template <typename T>
class X
   {
     public:
          virtual void do_toupper(const T & t) const = 0;
   };

template <typename T>
class Y : public X<T>
   {
     public:
      // Note that if this constructor is not explicit then a compiler-generated version 
      // of it is seen in the global scope but not added to the global declarations (currently).
      // It is added as a member function to the AST and a symbol is added to the symbol table 
      // in the scope. Then this is added as compiler generated, it still has it's EDG name 
      // (e.g. "Y___L16")
      // Y();// {}

         virtual void do_toupper(const T & t) const;
   };

Y<int> y;

#if 0
inline
void
X<char>::do_toupper(const char & c) const
   {
   }
#endif

