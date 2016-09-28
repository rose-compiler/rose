

template <class T> class X
   {
     public: T variable;
   };

#if 1
template class X<char>;
#else
extern template class X<char>;
#endif
