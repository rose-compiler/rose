template <class T>
class X
   {
     public:
          template<bool Y=0> X(T* abc, bool def=Y);

          template<bool B = true >
          operator X<int> () const
             {
               return X<int>(0L,0L,0L); 
             }
    };
