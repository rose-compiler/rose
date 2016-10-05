// This test code generates a namespace name of _GLOBAL__N__19_test2013_apps_11_cc_b0e478c3
// for the un-named namespace.  Any simpler test code will not generate the specially
// named namespace.

#include "vector"

#if 0
// An attempt to build a simpler vector defintion failed to generate the namespace with the special name.
namespace std 
{
template <typename T> 
class vector 
   {
      T t;
   };
}
#endif

namespace google {
namespace protobuf {

class DescriptorPool { public: class Tables; };

namespace 
   {

     struct Symbol {};

  // typedef std::vector<Symbol> SymbolsByNameMap;
  // typedef vector<Symbol> SymbolsByNameMap;
     typedef std::pair<int,Symbol> SymbolsByNameMap;

   }  // anonymous namespace

class DescriptorPool::Tables 
   {

     private:
          SymbolsByNameMap      symbols_by_name_;
   };

}  // namespace protobuf
}  // namespace google

