#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <set>

#include <boost/foreach.hpp>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"
#include "RtedVisit.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;



/* -----------------------------------------------------------
 * Run frontend and return project
 * -----------------------------------------------------------*/
SgProject*
RtedTransformation::parse(int argc, char** argv) {
   SgProject* project = frontend(argc, argv);
   ROSE_ASSERT(project);
   return project;
}

/* -----------------------------------------------------------
 * Load all function symbols that are needed for transformations
 * Basically we need to know which functions to insert
 * -----------------------------------------------------------*/
void RtedTransformation::loadFunctionSymbols(SgProject* project) {
   // traverse the AST and find locations that need to be transformed
   symbols->traverse(project, preorder);

   ROSE_ASSERT(symbols->roseCreateHeap);
   ROSE_ASSERT(symbols->roseAccessHeap);
   ROSE_ASSERT(symbols->roseFunctionCall);
   ROSE_ASSERT(symbols->roseAssertFunctionSignature);
   ROSE_ASSERT(symbols->roseConfirmFunctionSignature);
   ROSE_ASSERT(symbols->roseFreeMemory);
   ROSE_ASSERT(symbols->roseReallocateMemory);
   ROSE_ASSERT(symbols->roseConvertIntToString);
   ROSE_ASSERT(symbols->roseCheckpoint);
   ROSE_ASSERT(symbols->roseCreateVariable);
   ROSE_ASSERT(symbols->roseCreateObject);
   ROSE_ASSERT(symbols->roseInitVariable);
   ROSE_ASSERT(symbols->roseMovePointer);
   ROSE_ASSERT(symbols->roseAccessVariable);
   ROSE_ASSERT(symbols->roseEnterScope);
   ROSE_ASSERT(symbols->roseExitScope);
   ROSE_ASSERT(symbols->roseIOFunctionCall);
   ROSE_ASSERT(symbols->roseRegisterTypeCall);
   ROSE_ASSERT(symbols->size_t_member);
   ROSE_ASSERT(symbols->roseCheckIfThisNULL);
}

/* -----------------------------------------------------------
 * Perform all transformations needed
 * -----------------------------------------------------------*/
void RtedTransformation::transform(SgProject* project, set<string> &rtedfiles) {
   if (RTEDDEBUG())   cout << "Running Transformation..." << endl;
   globalScope = getFirstGlobalScope(isSgProject(project));
   ROSE_ASSERT( project);

   this -> rtedfiles = &rtedfiles;
   loadFunctionSymbols(project);

   VariableTraversal varTraversal(this);
   InheritedAttribute inheritedAttribute(false,false,false,false,false,false);
   //   InheritedAttribute inheritedAttribute(bools);
   // Call the traversal starting at the project (root) node of the AST
   varTraversal.traverseInputFiles(project,inheritedAttribute);


   // tps: Traverse all classes that appear in header files and create copy in source file within a namespace.
   // We need to know the sizeOf classes. To do so we need to modify the class but do not want to do this in the header file right now.
   vector<SgClassDeclaration*> traverseClasses;
   insertNamespaceIntoSourceFile(project,traverseClasses);
   // traverse all header files and collect information
   vector<SgClassDeclaration*>::const_iterator travClassIt = traverseClasses.begin();
   for (;travClassIt!=traverseClasses.end();++travClassIt) {
      // traverse the new classes with RTED namespace
      traverse(*travClassIt,preorder);
   }

   executeTransformations();
}


/* -----------------------------------------------------------
 * Collects information needed for transformations
 * -----------------------------------------------------------*/

void RtedTransformation::visit(SgNode* n) {
   if (RTEDDEBUG())  cerr <<"Traversing node : " << n->class_name() << endl;

   // find function definitions (incl. main) ******************************************
   if (isSgFunctionDefinition(n)) {
#if 0
      visit_isFunctionDefinition(n);
#endif
   }
   // find function definitions (incl. main) ******************************************


   // ******************** DETECT functions in input program  *********************************************************************

   // *********************** DETECT variable creations ***************
   if (isSgVariableDeclaration(n)) {
#if 0
      visit_isSgVariableDeclaration(n);
#endif
   }

   // *********************** DETECT variable creations ***************




   // *********************** DETECT ALL array creations ***************
   else if (isSgInitializedName(n)) {
#if 0
      //cerr <<" >> VISITOR :: Found initName : " << n->unparseToString() << endl;
      visit_isArraySgInitializedName(n);
#endif
   }

   // 1. look for MALLOC
   // 2. Look for assignments to variables - i.e. a variable is initialized
   // 3. Assign variables that come from assign initializers (not just assignments
   else if (isSgAssignOp(n)) {
#if 0
      //cerr <<" >> VISITOR :: Found AssignOp : " << n->unparseToString() << endl;
      visit_isArraySgAssignOp(n);
#endif
   }
   else if (isSgAssignInitializer(n)) {
      visit_isAssignInitializer(n);
   }
   // *********************** DETECT ALL array creations ***************


   // *********************** DETECT ALL array accesses ***************
   else if (isSgPntrArrRefExp(n)) {
      // checks for array access
      visit_isArrayPntrArrRefExp(n);
   } // pntrarrrefexp
   /*
  else if (isSgVarRefExp(n)) {
    // if this is a varrefexp and it is not initialized, we flag it.
    // do only if it is by itself or on right hand side of assignment
    cerr << " @@@@@@@@@ DETECTED Variable access : " << n->unparseToString() << endl;
    visit_isSgVarRefExp(isSgVarRefExp(n));
  }
    */
   else if (isSgPointerDerefExp(n)) {
      // if this is a varrefexp and it is not initialized, we flag it.
      // do only if it is by itself or on right hand side of assignment
      if (RTEDDEBUG()) cerr << " @@@@@@@@@ DETECTED PointerDerefExp : " << n->unparseToString() << endl;
      visit_isSgPointerDerefExp(isSgPointerDerefExp(n));
   }
   else if (isSgArrowExp(n)) {
      // if this is a varrefexp and it is not initialized, we flag it.
      // do only if it is by itself or on right hand side of assignment
      if (RTEDDEBUG()) cerr << " @@@@@@@@@ DETECTED isSgArrowExp : " << n->unparseToString() << endl;
      visit_isSgArrowExp(isSgArrowExp(n));
   }
   // *********************** DETECT ALL array accesses ***************


   // *********************** DETECT ALL scope statements ***************
   else if (isSgScopeStatement(n)) {
      // if, while, do, etc., where we need to check for locals going out of scope
      visit_isSgScopeStatement(n);
      // *********************** DETECT structs and class definitions ***************
      if (isSgClassDefinition(n)) {
         // call to a specific function that needs to be checked
         //cerr << " +++++++++++++++++++++ FOUND Class Def!! ++++++++++++++++ " << endl;
         visit_isClassDefinition(isSgClassDefinition(n));
      }

   }

   // *********************** DETECT ALL function calls ***************
   else if (isSgFunctionCallExp(n)) {
      // call to a specific function that needs to be checked

      visit_isFunctionCall(n);
   }
   // *********************** DETECT ALL function calls ***************


   // *********************** Detect pointer movements, e.g ++, -- *********
   else if(  isSgPlusPlusOp( n )
         || isSgMinusMinusOp( n )
         || isSgMinusAssignOp( n )
         || isSgPlusAssignOp( n )) {
#if 0
      visit_pointer_movement( n );
#endif
   }
   // *********************** Detect pointer movements, e.g ++, -- *********

   // *********************** Detect delete (c++ free) *********
   else if( isSgDeleteExp( n )) {
#if 0
      visit_delete( isSgDeleteExp( n ));
#endif
   }
   // *********************** Detect delete (c++ free) *********

   else if (isSgReturnStmt(n)) {
#if 0
      SgReturnStmt* rstmt = isSgReturnStmt(n);
      SgExpression* right = rstmt->get_expression();
      if (right)  // && !isSgValueExp(right))
         returnstmt.push_back(rstmt);
#endif
   }

   else {
      // cerr << " @@ Skipping : " << n->unparseToString() << "   " << n->class_name() << endl;

   }


   // ******************** DETECT functions in input program  *********************************************************************

}

#endif

// vim:et sta ts=2 sw=2:
