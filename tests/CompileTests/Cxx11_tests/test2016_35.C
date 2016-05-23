template <class T>
class X
   {
     public:
          template<typename C>
          operator X<C> () const
             {
               return X<int>(0L); 
             }
   };
