template<typename ObjectType>
class X
   {
     public:
       // Typedefs to define the pointer to member function types.
          typedef void (ObjectType::*AccessorFunctionType)();

         ObjectType *object;
          AccessorFunctionType mFieldAccessorMethod;

          void getRemapField();
   };

template<typename ObjectType>
void X<ObjectType>::getRemapField()
   {
     mFieldAccessorMethod = 0;
     (object->*(mFieldAccessorMethod))();
   }

class Y
   {
     public:
        void foo() {};
     // If uncommented, then getRemapField() will record that both functions are (or could be) called.
     // void foobar() {};
   };


int main()
   {
     X<Y> x;
     x.getRemapField();

     return 0;
   } 

