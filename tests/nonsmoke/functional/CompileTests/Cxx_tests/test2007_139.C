/*
Bug name:	spec-problems-current/interaction-between-inlined-method-and-difference-type

Reported by:	Brian White

Date:		Sept 30, 2007

Date sent:      Sept 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 	src/483.xalancbmk/src/XPathExpression.hpp

Brief description:  	Invocation of std::vector<int>::difference_type
                        constructor within a method inlined within a class
                        causes the following assertion failure:

In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer 
rosec: Cxx_Grammar.C:5567: bool Sg_File_Info::isCompilerGenerated() const: Assertion `this != __null' failed.
Abort (core dumped)

                        The assertion failure does not occur if
                        the method is not inlined (i.e., un-#define INLINED).

Files
-----
bug.cc		Invokes std::vector<int>::difference_type constructor,
                passing it (x-x), where x is a std::vector<int>::const_iterator.
                If the invoking method (doSomething) is not inlined,
                no assertion failure occurs.
*/

#define INLINED

#include <vector>

class foo {

public:
  
#ifdef INLINED
  void doSomething() {
    std::vector<int>::const_iterator x;
    std::vector<int>::difference_type diff = std::vector<int>::difference_type(x-x);
  }
#else
  void doSomething();
#endif

};

#ifndef INLINED
void foo::doSomething()
{
  std::vector<int>::const_iterator x;
  std::vector<int>::difference_type diff = std::vector<int>::difference_type(x-x);
}
#endif
