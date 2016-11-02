// This test code demonstrates the required specialization of "template <> int X<int>::foo()"
// This test is very similar to test2005_136.C except that the template member function is 
// defined outside of the class.

#define REQUIRED 1

// ****************************
// Member function form of test
// ****************************

// Template class containing member function (defined outside the class)
template <typename T>
class X
   {
     private:
          T t;
   };

// specialization
template<>
class X<int>
   {
     private:
          int t;
   };

int main()
   {
     X<long> a;
     X<int> b;
   }

