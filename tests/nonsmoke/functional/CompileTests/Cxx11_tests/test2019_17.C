template<class T>
struct Myclass
   {
     static int xyz;
   };

int abc = 42;

template<class T> int Myclass<T>::xyz = abc+42;

int def = Myclass<int>::xyz;

template int Myclass<int>::xyz;

int ghi = Myclass<int>::xyz;

void foobar()
   {
     int x = Myclass<int>::xyz + Myclass<long>::xyz;
   }

