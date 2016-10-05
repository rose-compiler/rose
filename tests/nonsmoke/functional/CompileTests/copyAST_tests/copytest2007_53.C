// test2005_150.C

// Debug ambiguous template specialization if template <> X :: X() {}

template <typename T>
class X
   {
     private:
          T t;
     public:
          X(T t) : t(t) {};

          template <typename S> X(S t) {};      
   };

#if 0
// This is OK to run through EDG, but g++ can't resolve which function this is a specialization for!
template <>
inline X < int > ::X(int t) : t(t)
{
}
#endif

int main()
   {
     X<int> a(1);
     X<int> b(1.0);
   }

