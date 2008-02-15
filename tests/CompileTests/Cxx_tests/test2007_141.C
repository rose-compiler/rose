/*
Bug name:	interaction-between-inlined-method-and-map

Reported by:	Brian White

Date:		Sept 30, 2007

Date sent:      Sept 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 	src/483.xalancbmk/src/Stylesheet.hpp

Brief description:  	Invocation of operator== on two actuals of type
                        std::map<int,int>::iterator
                        within a method inlined within a class
                        causes the following assertion failure:

In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer 
rosec: Cxx_Grammar.C:5567: bool Sg_File_Info::isCompilerGenerated() const: Assertion `this != __null' failed.

                        The assertion failure does not occur if
                        the method is not inlined (i.e., un-#define INLINED).

                        This bug is presumably the same as
                        that reported in 
                        interaction-between-inlined-method-and-difference-type.

Files
-----
bug.cc		Invokes comparison on two std::map<int,int>::iterator vars.

                If the invoking method (doSomething) is not inlined,
                no assertion failure occurs.
*/

#define INLINED

#include <map>

class foo {

public:
  
#ifdef INLINED
  int doSomething() {
    std::map<int, int>::iterator it;
    return ( it == it );
  }
#else
  int doSomething();
#endif

};

#ifndef INLINED
int foo::doSomething()
{
    std::map<int, int>::iterator it;
    return ( it == it );

}
#endif
