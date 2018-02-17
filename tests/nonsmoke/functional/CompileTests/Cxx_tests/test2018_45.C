template<class T> struct X 
   {
     struct Y {};
  // static Y yyy;
   };

class Z
   {
     template<class T> friend struct X<T>::Y;
  // template<class T> friend struct X<T>::yyy;
   };

