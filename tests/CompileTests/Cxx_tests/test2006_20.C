#if 0
This bug was found while compiling
/src/midend/astUtil/astInterface/AstInterface.C. I was able to make it
disappear by changing the code as specified below, but still it is a bug
since
__uninitialized_copy_aux is a valid template in STL and is found by GCC.

Compiling the following code using ROSE:
//Support header files for stl_uninitialized.h
#include <bits/stl_algobase.h>
#include <bits/stl_construct.h>
//The __uninitialized_copy_aux template is declared and defined within
stl_uninitialized.h
#include <bits/stl_uninitialized.h>

using namespace std;
class BoolAttribute
{};

//To make the bug disappear just replace __uninitialized_copy_aux with
std::__uninitialized_copy_aux
template BoolAttribute * __uninitialized_copy_aux<BoolAttribute const *,
BoolAttribute *>(BoolAttribute const *, BoolAttribute const *,
BoolAttribute *, __false_type);


gives the following error:

"../../../../../ROSE/src/midend/astUtil/astInterface/AstInterface.C", line
13: error:
          "__uninitialized_copy_aux" is not a class or function template name
          in the current scope
  template BoolAttribute * __uninitialized_copy_aux<BoolAttribute const *,
BoolAttribute *>(BoolAttribute const *, BoolAttribute const *,
BoolAttribute *, __false_type);

Andreas
#endif

// Support header files for stl_uninitialized.h
#include <bits/stl_algobase.h>
#include <bits/stl_construct.h>
// The __uninitialized_copy_aux template is declared and defined within stl_uninitialized.h
#include <bits/stl_uninitialized.h>

using namespace std;
class BoolAttribute
{};

// DQ (7/3/2013): This is part of a temporary fix to work around a bug in EDG 4.7, forcing us 
// to tell EDG we are an EDG 4.2 compiler to avoid bug in Boost (see test2013_246.C).
// DQ (9/12/2009): std::__uninitialized_copy_aux is not present in GNU g++ version 4.3.
// #if ( (__GNUC__ == 3) || (__GNUC__ == 4) && (__GNUC_MINOR__ < 3) )
// #if ( (__GNUC__ == 3) || (__GNUC__ == 4) && (__GNUC_MINOR__ < 3) ) && !defined(LIE_ABOUT_GNU_VERSION_TO_EDG)
#if ( defined(__clang__) == 0 && ((__GNUC__ == 3) || (__GNUC__ == 4) && (__GNUC_MINOR__ < 3) ) && !defined(LIE_ABOUT_GNU_VERSION_TO_EDG) )
// DQ (8/14/2006): Fixing this by adding "std::" is completely appropriate since it is
// what was required to permit the Intel compiler to handle ROSE and it is really an 
// EDG issue anyway.  So this is the fix we have selected.
// To make the bug disappear just replace __uninitialized_copy_aux with std::__uninitialized_copy_aux
template BoolAttribute * std::__uninitialized_copy_aux<BoolAttribute const *,BoolAttribute *>(BoolAttribute const *, BoolAttribute const *, BoolAttribute *, __false_type);
#endif
