template<typename T>
struct X
   {
   };

template<typename T>
struct Y : X<T>
   {
   };

int main()
   {
     X<int>   i;
     X<float> j;

     return 0;
   }

