// class W;

template<class T> struct X 
   {
     struct Y { };
   };

class Z
   {
     template<class T> friend struct X<T>::Y;
   };

