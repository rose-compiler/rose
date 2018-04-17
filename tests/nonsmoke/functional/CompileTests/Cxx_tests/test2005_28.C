#if 0
// Original Code:
   vector<int>::const_iterator function;
   vector<int> functionList = getEOSDatabase()->getUniqueFunctionList();
   for (function = functionList.begin();
        function != functionList.end();
        ++function)
   {
      vector<double> minmax;

// Generated Code:
  std::vector < int , std::allocator< int > > ::const_iterator function;
  class std::vector< int  , std::allocator< int  >  > functionList(( *(this) -> getEOSDatabase()).getUniqueFunctionList());
  for (function = __gnu_cxx::__normal_iterator < std::vector < int , std::allocator< int > > ::const_pointer , std::vector < int , std::allocator< int > > ::vector_type > (((functionList.begin()))); function!=((functionList.end())); ++function) {
    class std::vector< double  , std::allocator< double  >  > minmax;


function = functionList.begin();
function = __gnu_cxx::__normal_iterator < std::vector < int , std::allocator< int > > ::const_pointer , std::vector < int , std::allocator< int > > ::vector_type > (((functionList.begin())));
#endif


#include <vector>

std::vector<int> intList;
// std::vector<int>::const_iterator intListInterator = intList.begin();

void foo()
   {
     std::vector<int>::const_iterator intListInteratorDirectAssignment = intList.begin();
     std::vector<int>::const_iterator intListInterator;
     intListInterator = intList.begin();
   }
 


