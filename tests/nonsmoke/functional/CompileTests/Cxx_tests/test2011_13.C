/*

Dan has confirmed that this is a bug; every defining declaration should have a non-NULL definition attached.

//The function MIN here has a defining declaration, but a NULL definition
----------------------------------------------
template <class T>
const T& MIN( const T& a, const T& b )
 {
   return b < a ? b : a;
 }

class c
{
     void foo()
       {
         MIN(1, 2);
       }
 };
----------------------------------------------



To verify this is the case, you can use the following translator

----------------------------------------------
#include "rose.h"
#include <boost/foreach.hpp>
int main(int argc, char** argv)
{
    SgProject* project = frontend(argc, argv);

    vector<SgFunctionDeclaration*> functions = SageInterface::querySubTree<SgFunctionDeclaration>(project,
V_SgFunctionDeclaration);
    BOOST_FOREACH(SgFunctionDeclaration* function, functions)
    {
        //Process each function only once
        if (function != function->get_firstNondefiningDeclaration())
            continue;

        function = isSgFunctionDeclaration(function->get_definingDeclaration());
        if (function != NULL)
            ROSE_ASSERT(function->get_definition() != NULL);
    }
    return 0;
}
---------------------------------------------- 

*/

template <class T>
const T& MIN( const T& a, const T& b )
   {
     return b < a ? b : a;
   }

class c
   {
     void foo()
       {
         MIN(1, 2);
       }
   };
