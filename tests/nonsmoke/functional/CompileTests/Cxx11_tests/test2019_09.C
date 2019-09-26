// Unclear if this is legal code!  I think it is not legal code!

template <class T> class Array;

// NOTE: reference to bar() in class Array is likely resolved to this function.
template <class T> void bar(Array<T> &);

template <class T>
class Array 
   {
  // This is an error for GNU 5.1, but passes EDG 4.12.  Not clear which function is should be refering to...
     friend void bar<T>(Array<T> &); // << NEEDED

     public: template <class T> void bar(Array<T> &);
   };

