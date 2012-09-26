template <typename T1>
class pair
   {
   };

// Note that the return type "pair<_Tp*>" is a template class instantiation, I think this is correct.
template<typename _Tp>
pair<_Tp*> foo()
   {
     return pair<_Tp*>();
   }

#if 1
template<typename _Tp>
pair<_Tp*> foobar()
   { 
     return pair<_Tp*>();
   }
#endif
