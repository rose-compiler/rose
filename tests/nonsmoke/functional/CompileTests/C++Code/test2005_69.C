#if 0
// forward declaration of class
class B;

// Can't build forward declaration of member function of class (not yet seen)
// void B::f(int y);

class B
   {
         int x;
     public:
         void f(int y)
            {
              x=x+y;            
            }
   };

// error: member function "B::f" may not be redeclared outside its class
// void B::f(int y);
#endif

// forward declaration of templated class (this is legal C++)
// template<class U> class A;

// Can't build forward declaration of member function of templated class (not yet seen)
// template<class U> void A<U>::f(int y);

#define DEFINE_FUNCTION_IN_CLASS

template<class U>
class A
   {
         U x;
     public:
         static U myVar;
         static float myFloatVar;
         
     public:
#if 0
      // templates defined in the class are preserved in the output template declaration
         template<class T> void g(int y, T u) 
            {
              y = y+u;
              x+=u;
              
            }
#endif
      // non-template member functions are not represented in the templated class 
      // (even though we have specified FRIEND_AND_MEMBER_DEFINITIONS_MAY_BE_MOVED_OUT_OF_CLASS 
      // to be FALSE in EDG).
         void f(int y)
#ifdef DEFINE_FUNCTION_IN_CLASS
            {
              x=x+y;
            }
#else
            ;
#endif
   };

// This is a template static data member (SgVariable)
template<class U> U A<U>::myVar;

// This is a template static data member (SgVariable)
template<class U> float A<U>::myFloatVar;

// template specialization for variable (was originally defined to be float!)
template<> float A<double>::myFloatVar;

// template specialization for variable (this is not possible)
// template<> float A<double>::myVar;

// error: member function "A<U>::f" may not be redeclared outside its class
// template<class U> void A<U>::f(int y);

#ifndef DEFINE_FUNCTION_IN_CLASS
// If not defined in the class then we can define it as a template outside of the class
// template<class U> void A<U>::f(int y) {};
#endif

// Specialization of template<class U> class A
template<> class A<float>
   {
   };

#if 1
// define the following specialization
inline void A < int > ::f(int y)
{
  myVar = 42;
  myFloatVar = 3.14;
  (this) -> x = (this) -> x + y;
}
// define the following specialization
inline void A < double > ::f(int y)
{
  myVar = 42;
  myFloatVar = 3.14;
  (this) -> x = (this) -> x + y;
}
#endif

// explicit template instantiation directive
// template void A<int>::g<int>(int y, int u);
// template void A<int>::f(int y);

int main()
   {
     A<int> a;
     int x,y;
  // a.g<int>(x,y);   
  // a.f(x);
     A<double> b;
   }

// Unparser will generate the following output (which g++ reports 
// as an error: "error: redefinition of `double A<double>::myVar'")
// double A < double > ::myVar;
// int A < int > ::myVar;
// float A < int > ::myFloatVar;
