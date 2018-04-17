

template <typename T>
class X
   {
     int x;
   };

#if 1
template <>
class X < int >  
   {
     int x;
   };

// This can only be defined once!
// template <> class X < int > { int x; };
#else
template class X < int >;
#endif
