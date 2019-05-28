template<class T>
struct Myclass
   {
     static int xyz;
   };

template<class T> int Myclass<T>::xyz = 42;

// Template instantiation directive missing name qualification.
// Should be: template int Myclass<int>::xyz;
// Is unparsed as: template int xyz;
template int Myclass<int>::xyz;

