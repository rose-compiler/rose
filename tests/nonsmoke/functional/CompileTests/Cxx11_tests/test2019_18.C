template<class T>
struct Myclass
   {
     static int xyz;
   };

int abc = 42;

template<class T> int Myclass<T>::xyz = abc--;

template int Myclass<int>::xyz;

void foobar()
   {
   }
