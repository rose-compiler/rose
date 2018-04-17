// This example code fails to link, but it will compile!
// It will compile and link with g++, just not with ROSE.

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
   };


int main()
   {
     X<Y> x;
     x.getRemapField();

     return 0;
   }



