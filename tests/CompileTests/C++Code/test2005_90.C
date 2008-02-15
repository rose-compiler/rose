// Previous code from Kyle demonstarated that templated base class names are not
// converted to specialized template names


template <typename T>
class A
   {
   };

class B : public A<int>
   {
   };

int
main()
   {
     B b;
   }




