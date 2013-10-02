// Example of template specialization of template class 
// (adding a new member function not in the base template declaration).

template<class T> class A {
   public:
      void f(T);
};

template<> class A<int> {
   public:
      int g(int);
};

// This is unparsed as: template<> int A< int > ::g(int arg)
// g++ reports: error: template-id 'g<>' for 'int A<int>::g(int)' does not match any template declaration
int A<int>::g(int arg) 
   { 
     return 0; 
   }

int main() 
   {
     A<int> a;
     a.g(1234);
   }

