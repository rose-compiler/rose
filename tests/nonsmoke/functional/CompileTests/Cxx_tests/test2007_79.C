/*
Hi Dan,

I have another bug report for you:

$ cat test.cpp
#include <vector>

class Tester {
   int mVal;

   void swap(Tester&       theOther)
   {
       std::swap(mVal, theOther.mVal);
   }
};


stonea@harpo ~/ma/rose/xalanc
$ g++ -c test.cpp

stonea@harpo ~/ma/rose/xalanc
$ rosec test.cpp
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

-Andy 
*/


#include <vector>

class Tester {
   int mVal;

   void swap(Tester&       theOther)
   {
       std::swap(mVal, theOther.mVal);
   }
};

