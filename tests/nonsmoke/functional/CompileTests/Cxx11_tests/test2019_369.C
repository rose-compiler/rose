struct S 
   {
     static enum { No, Yes } locked;
   };

template<class T> int f(T) { return (int)(T::Yes); }

template int f( decltype(S::locked) );
