 #include <vector>

class MemoryPage { int size; public: MemoryPage(int s) : size(s) {} };

using namespace std;
int main()
   {
     vector<MemoryPage> vm;
     MemoryPage mp(1024);//lvalue
     vm.push_back(mp); //push_back(const T&)

  // However, you can enforce the selection of push_back(T&&) even in this case by casting an lvalue to an rvalue reference using static_cast:
  // calls push_back(T&&)
     vm.push_back(static_cast<MemoryPage&&>(mp));

  // Alternatively, use the new standard function std::move() for the same purpose:
     vm.push_back(std::move(mp));//calls push_back(T&&)
   }


