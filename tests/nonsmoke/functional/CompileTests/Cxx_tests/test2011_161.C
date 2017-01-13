template<class T>
class DEF
   {
     public:
          T xyz;
          T foo ();
   };

template<class T>
T DEF<T>::foo ()
   { 
     return xyz;
   }

#if 0
int main()
   {
     DEF<int> object2;
     object2.xyz = 8;
     object2.xyz = object2.foo();

     return 0;
   }
#endif

