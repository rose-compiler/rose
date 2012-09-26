template <typename T1>
class Y
   {
   };

// Note that "T<_Tp*>" should be a template class instantiation (instead 
// it points to the template declaration). I think this is NOT correct.
template<typename _Tp>
class X : Y<_Tp*>
   {
   };


