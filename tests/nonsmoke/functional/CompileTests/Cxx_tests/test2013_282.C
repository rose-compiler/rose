// DQ (12/1/2013): Reported bug from last week.
// This is a reported bug.  However it only fails using g++ version 4.6
// where it will pass only using the -fpermissive flag in the backend 
// compilation.  So it appears to be a code generation issue that is
// specific to a detail that is being tightened up within g++.

#define GOOD
#ifdef GOOD
#include <vector>

int main(int argc, char** argv){

	std::vector<int> v = std::vector<int>();
	
return 0;
}

#else

#include <vector>
namespace std
{
}

int main(int argc,char **argv)
{
std::vector< int  , class std::allocator< int  >  > v = std::vector< int ,class std::allocator< int  > > ::vector(); // this fails in g++
//      v.push_back(5);
return 0;
}
#endif
