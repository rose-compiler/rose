// #include<list>

extern "C" void _Exit(int);
namespace __gnu_cxx
   {
  // inline long long abs(long long __x) { return 0; }
   }

namespace __gnu_cxx
   {
     extern "C" void (_Exit)(int);
     using ::_Exit;
     inline long long abs(long long __x) { return 0; }
   }

namespace std
   {
  // using ::__gnu_cxx::abs;
   }

namespace std
   {
      using ::__gnu_cxx::_Exit;
      using ::__gnu_cxx::abs;
   }


