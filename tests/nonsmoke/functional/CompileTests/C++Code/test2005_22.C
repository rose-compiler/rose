/*
Hi Dan,

You told me to send you a simple program showing that using MiddleLevelRewrite::insert() is not as easy to use as it could be.

In this program I'm trying to use MiddleLevelRewrite::insert to insert a simple "int z;" before the return statement of the function. The execution of the code does produce an output file, but this file doesn't seem to be correct. (The file is listed below.)


In addition I get an error message before the execution aborts:

myexe: Cxx_Grammar.C:1723: static char** SgNode::buildCommandLineToSubstituteTransformationFile(int, char**, std::basic_string<char, std::char_traits<char>, std::allocator<char> >): Assertion `fileNameCounter == 1' failed.
Abort

(and it's not easy to figure out how to fix this...)


I'm not convinced that I am using the MiddleLevelRewrite function correctly. Maybe I should be using a AstTopDownBottomUpProcessing instead of SgSimpleProcessing?

(BTW, I'm moving back to Rich's office after lunch. After all I am now allowed to use that computer...)


Vera


Listing of files:
----------------------

Input file:
-------------
int main(){

 int y;
 y++;

 return 0;
}

Output file:
---------------
*/

#if 0
#include "rose.h"
int main()

{ /* local stack #0 */
int y;
int GlobalScopePreambleStart;
int GlobalScopePreambleEnd;
int CurrentLocationTopOfScopeStart;
int CurrentLocationTopOfScopeEnd;
int CurrentLocationBeforeStart;
int z_; int CurrentLocationBeforeEnd;
int CurrentLocationReplaceStart;
int CurrentLocationReplaceEnd;
int CurrentLocationAfterStart;
int CurrentLocationAfterEnd;
int CurrentLocationBottomOfScopeStart;
int CurrentLocationBottomOfScopeEnd;

/* Reference marker variables to avoid compiler warnings */
  };



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Insert.h"




void Insert:: visit(SgNode* astNode){
  // I want to insert a simple text line "int z;" before the return statement.

  // I'm sure the SgStatement I send as an inputparameter to MiddleLevelRewrite::insert is of 
  // type SgStatement. For the two last inputparameters I use the default values.
  
  if(isSgReturnStmt(astNode)!=NULL){
    if(isSgStatement(astNode)!=NULL){
      SgStatement *g = isSgStatement(astNode);
      cout << "g is a sgStatement and a return statement" << g->unparseToString() << endl;
      MiddleLevelRewrite::insert(g, "int z_:", MidLevelCollectionTypedefs::SurroundingScope,
                                 MidLevelCollectionTypedefs::BeforeCurrentPosition);
    } 
  }
}
X



#ifndef INSERT_IS_DEFINED
#define INSERT_IS_DEFINED



class Insert : public SgSimpleProcessing{

 public:
  void virtual visit(SgNode* node);

};

#endif



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "Insert.h"


int main(int argc, char *argv[]){
 

  SgProject *sgproject = frontend(argc, argv);
  ROSE_ASSERT(sgproject != NULL);
 
  // In this traversal I want to add a statement by inserting a string in the AST, using MidLevelRewrite.
  // This new statement will be placed before the last return statement.
  Insert p;
  p.traverse(sgproject,preorder);
    
  return 0;
}

/*
Executing this test program with the simple inputfile testfile.cpp, does produce an output file with the
name rose_transformation_testfile1.C. However, this output file is not as expected.

In addition, the execution is aborted after printing this error message:

myexe: Cxx_Grammar.C:1723: static char** SgNode::buildCommandLineToSubstituteTransformationFile(int, char**, std::basic_string<char, std::char_traits<char>, std::allocator<char> >): Assertion `fileNameCounter == 1' failed.
Abort

Should I be using AstTopDownBottomUpProcessing instead of SgSimpleProcessing?

You asked me to report if simple use of this rewrite mechanismn wasn't as easy to use as it could have been. So therefore this simple program and simple use of the insert function that doesn't work.

*/



int main()  

{ /* local stack #0 */ 
int y;  
int GlobalScopePreambleStart;
int GlobalScopePreambleEnd;
int CurrentLocationTopOfScopeStart;
int CurrentLocationTopOfScopeEnd;
int CurrentLocationBeforeStart;
int z_ int CurrentLocationBeforeEnd;
int CurrentLocationReplaceStart;
int CurrentLocationReplaceEnd;
int CurrentLocationAfterStart;
int CurrentLocationAfterEnd;
int CurrentLocationBottomOfScopeStart;
int CurrentLocationBottomOfScopeEnd;

/* Reference marker variables to avoid compiler warnings */ 
   };  



int main(){

  int y;
  y++;

  return 0;
}

#endif


