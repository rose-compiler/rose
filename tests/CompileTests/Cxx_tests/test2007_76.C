/*
Email from Andy Stone:

Hi,

Here's another bug report.  I don't think it's significant that the
template throws an exception, but it is significant that the
template-function is called in a member-function.


$ cat throw.cpp
template<class E> inline void throw_exception(E const & e)
{
   throw e;
}

class Foo {
   void foo() {
       throw_exception(3);
   }
};


stonea@monk ~/ma/rose/throwTest
$ g++ -c throw.cpp

stonea@monk ~/ma/rose/throwTest
$ rosec -c throw.cpp
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList):
found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration):
found a null file info pointer
rosec: Cxx_Grammar.C:5563: bool Sg_File_Info::isCompilerGenerated()
const: Assertion `this != __null' failed.
Aborted 
*/

template<class E> inline void throw_exception(E const & e)
{
   throw e;
}

class Foo {
   void foo() {
       throw_exception(3);
   }
};

