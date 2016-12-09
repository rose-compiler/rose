template<typename ObjectType>
class X
   {
     public:
       // Typedefs to define the pointer to member function types.
          typedef void (ObjectType::*AccessorFunctionType)();

         ObjectType *object;

       // Member function pointer
          AccessorFunctionType mFieldAccessorMethod;

          void getRemapField();
   };

#if 1
template<typename ObjectType>
void X<ObjectType>::getRemapField()
   {
  // Set ObjectType pointer.
     object = 0;

  // Set member function pointer to NULL.
     mFieldAccessorMethod = 0;

  // Call member function using member function pointer.
     (object->*(mFieldAccessorMethod))();
   }
#endif

class Y
   {
     public:
        void foo() {};
   };

int main()
   {
     X<Y> x;
     x.getRemapField();

     return 0;
   }

