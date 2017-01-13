template<class T>
class DEF
   {
     public:
          T xyz;

       // This will be interpreted as a non-template member function in a templated class, is this OK?
          T foo ();
   };

// This is interpreted as a template member function (inconsistant with the declaration in the template class?)
template<class T>
T DEF<T>::foo ()
   { 
     return xyz;
   }

// Note that the template class specialization should be unparsed as: template<> class DEF <int>

#if 1
int main()
   {
     DEF<int> object2;
  // object2.xyz = 8;
  // object2.xyz = object2.foo();

     return 0;
   }
#endif

