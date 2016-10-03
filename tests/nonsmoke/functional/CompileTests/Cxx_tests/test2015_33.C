// Test: ouput of temaple specialization.

template <typename T>
class A 
   {
     public:
          A();

     private:
          T _val;
   };

template<typename T>
A<T>::A()
   {
   }

int main() {
  A<int> a;
  return 0;
}
