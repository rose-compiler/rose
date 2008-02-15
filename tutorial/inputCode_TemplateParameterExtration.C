
// Templated class declaration used in template parameter example code
template <typename T>
class templateClass
   {
     public:
          int x;

          void foo(int);
          void foo(double);
   };


int main()
   {
     templateClass<char> instantiatedClass;
     instantiatedClass.foo(7);
     instantiatedClass.foo(7.0);

     templateClass<int> instantiatedClassInt;
     templateClass<float> instantiatedClassFloat;
     templateClass<templateClass<char> > instantiatedClassNestedChar;

     for (int i=0; i < 4; i++)
        {
          int x;
        }

     return 0;
   }
