template<class T>
class A
   {
     public:
       // This is an error since B is not in the list of classes that have been seen yet.
          class B
             {
             };
   };

