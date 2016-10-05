// number #27
namespace std
{
  template<typename T> class A{};
}

std::A<int> a1;

void foo()
   {
     std::A<float> a2;
   }
