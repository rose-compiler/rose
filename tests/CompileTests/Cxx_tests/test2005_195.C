
#if 0
When compiling the code:
// DQ (12/7/2003): g++ 3.x prefers to see <fstream> and use of <fstream>
//                 or <fstream.h> is setup in config.h so use it here.

ROSE gives the following error:

"../../../../../ROSE/src/midend/astUtil/astInterface/AstInterface.C", line
15: error:
          "__uninitialized_copy_aux" is not a class or function template name
          in the current scope
  template BoolAttribute * __uninitialized_copy_aux<BoolAttribute const *,
BoolAttribute *>(BoolAttribute const *, BoolAttribute const *,
BoolAttribute *, __false_type);
                           ^

"../../../../../ROSE/src/midend/astUtil/astInterface/AstInterface.C", line
16: error:
          "__uninitialized_copy_aux" is not a class or function template name
          in the current scope
  template BoolAttribute * __uninitialized_copy_aux<BoolAttribute *,
BoolAttribute *>(BoolAttribute *, BoolAttribute *, BoolAttribute *,
__false_type);
                           ^

Errors in EDG Processing!
/home/saebjornsen1/links/rose-rose-compile/g++3.3.3: line 4: 31082 Aborted

Andreas
#endif


// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include <fstream>
#include <list>
using namespace std;
#define Boolean int

class BoolAttribute
{
  Boolean val;
 public:
  BoolAttribute( Boolean v = true) : val(v) {}
  operator Boolean() const { return val; }
};

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

// The comment below now appears in ROSE/src/midend/astUtil/astInterface/AstInterface.C
// DQ (12/28/2005): ROSE can't compile these lines and they appear to not make any sense, 
// and it seems that we don't need them, so let's comment them out.
// template BoolAttribute * __uninitialized_copy_aux<BoolAttribute const *,BoolAttribute *>(BoolAttribute const *, BoolAttribute const *,BoolAttribute *, __false_type);
// template BoolAttribute * __uninitialized_copy_aux<BoolAttribute *,BoolAttribute *>      (BoolAttribute *, BoolAttribute *, BoolAttribute *,__false_type);
