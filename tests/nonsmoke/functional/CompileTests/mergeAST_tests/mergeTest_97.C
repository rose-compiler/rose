template<typename T, typename U>
class X
   {
     public:
          template<typename T1, typename T2> void foo(X<T1,T2> __is){};
   };

// This causes the SgStorageModifier is not in parent's child list, SgInitializedName
// Also a problem for the merge!
template void X<int,int>::foo(X<int,int> __is);

