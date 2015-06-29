template <template < typename TYPE> class DATACLASS, typename TYPE>
class XXX 
   {
     public:
          XXX();
   };

// This builds an instantiation of the class to support a scope into 
// which the member function is placed (in this case a constructor).
template <template <typename> class DATACLASS, typename TYPE >
XXX<DATACLASS, TYPE>::XXX()
   {
#if 0
     TYPE x;
#endif
   }
