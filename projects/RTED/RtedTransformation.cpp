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
 * Perform all transformations needed (Step 2)
 * -----------------------------------------------------------*/
void RtedTransformation::transform(SgProject* project, set<string> &rtedfiles) {
  if (RTEDDEBUG())
	  cout << "Running Transformation..." << endl;
  globalScope = getFirstGlobalScope(isSgProject(project));
  ROSE_ASSERT( project);

  this -> rtedfiles = &rtedfiles;
  loadFunctionSymbols(project);

  // Traverse Variables
  InheritedAttribute inheritedAttribute(false,false);
  VariableTraversal varTraversal(this);
 // Call the traversal starting at the project (root) node of the AST
  varTraversal.traverseInputFiles(project,inheritedAttribute);
  // traverse visit function further below
  traverseInputFiles(project,preorder);


  vector<SgClassDeclaration*> traverseClasses;
  //*******************************************
  // for all of the sourcefiles create a namespace at the top of the file
  // add to top of each source file
  vector<SgNode*> resSF = NodeQuery::querySubTree(project,V_SgSourceFile);
  // insert at top of all C files in reverse order
  // only if the class has a constructor and if it is declared in a header file
  vector<SgNode*>::const_iterator resSFIt = resSF.begin();
  for (;resSFIt!=resSF.end();resSFIt++) {
    SgSourceFile* sf = isSgSourceFile(*resSFIt);
    ROSE_ASSERT(sf);
    bool isInSourceFileSet = isInInstrumentedFile(sf);
    if (isInSourceFileSet) {
      // we should only do this for C++!
      std::string filename = sf->get_file_info()->get_filename();
      if ((filename.find(".cxx")!=std::string::npos ||
                   filename.find(".cpp")!=std::string::npos ||
		   filename.find(".C")!=std::string::npos  ) ) {
        // if it is not a C but C++ program, then insert namespace
    	  if (RTEDDEBUG()) cerr << " **** Inserting file into sourceFileRoseNamespaceMap:" << sf -> get_file_info() -> get_filename() << endl;
	  //if (filename.find("_s.cpp")!=std::string::npos)
    	  insertNamespaceIntoSourceFile(sf);
      } else {
    	//  cerr << " ** not a cpp file" <<filename << endl;
      }
    }
  }
  cerr << "Deep copy of all C++ class declarations to allow offsetof to be used." << endl;
  vector<SgNode*> results = NodeQuery::querySubTree(project,V_SgClassDeclaration);
  // insert at top of all C files in reverse order
  // only if the class has a constructor and if it is declared in a header file
  // tps (11/06/2009) : it seems that the reverse iterator does not work on MAC OS, so I added another loop to get the reverse vector
  vector<SgNode*>::const_iterator classItR = results.begin();
  vector<SgNode*> resultsInv;
  for (;classItR!=results.end();classItR++) {
    resultsInv.insert(resultsInv.begin(),*classItR);
  }
  ROSE_ASSERT(resultsInv.size()==results.size());
  vector<SgNode*>::const_iterator classIt = resultsInv.begin();
  for (;classIt!=resultsInv.end();classIt++) {
    SgClassDeclaration* classDecl = isSgClassDeclaration(*classIt);
    if (classDecl->get_definingDeclaration()==classDecl)
      if (!classDecl->get_file_info()->isCompilerGenerated()) {
	string filename = classDecl->get_file_info()->get_filenameString();
	size_t idx = filename.rfind('.');
	std::string extension ="";
	if(idx != std::string::npos)
	  extension = filename.substr(idx+1);
	if ((extension!="C" && extension!="cpp" && extension!="cxx") &&
		      	filename.find("include-staging")==string::npos &&
			filename.find("/usr/include")==string::npos
	) {
	  std::vector<std::pair<SgNode*,std::string> > vec = classDecl->returnDataMemberPointers();
	  if (RTEDDEBUG()) cerr << "\n ** Deep copy: Found classDecl : " << classDecl->get_name().str() << "  in File: " << filename <<
	    "    with number of datamembers: " << vec.size() << "   defining " <<
	    (classDecl->get_definingDeclaration()==classDecl) << endl;
	  if (hasPrivateDataMembers(classDecl)) {
	    /*SgClassDeclaration* cdl = */instrumentClassDeclarationIntoTopOfAllSourceFiles(project, classDecl);
	    //traverseClasses.push_back(cdl);
	  } //else
	    traverseClasses.push_back(classDecl);
	} else if (
			filename.find("include-staging")==string::npos ||
			filename.find("/usr/include")==string::npos) {
			std::string classname = classDecl->get_name().str();
	} 
      }
  }
  moveupPreprocessingInfo(project);


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
	cerr <<"Traversing node : " << n->class_name() << endl;

  // find function definitions (incl. main) ******************************************
  if (isSgFunctionDefinition(n)) {
    visit_isFunctionDefinition(n);
  }
  // find function definitions (incl. main) ******************************************


  // ******************** DETECT functions in input program  *********************************************************************

  // *********************** DETECT variable creations ***************
  if (isSgVariableDeclaration(n)) {
    visit_isSgVariableDeclaration(n);
  }

  // *********************** DETECT variable creations ***************




  // *********************** DETECT ALL array creations ***************
  else if (isSgInitializedName(n)) {
    //cerr <<" >> VISITOR :: Found initName : " << n->unparseToString() << endl;
    visit_isArraySgInitializedName(n);
  }

  // 1. look for MALLOC 
  // 2. Look for assignments to variables - i.e. a variable is initialized
  // 3. Assign variables that come from assign initializers (not just assignments
  else if (isSgAssignOp(n)) {
    //cerr <<" >> VISITOR :: Found AssignOp : " << n->unparseToString() << endl;
    visit_isArraySgAssignOp(n);
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
    cerr << " @@@@@@@@@ DETECTED PointerDerefExp : " << n->unparseToString() << endl;
    visit_isSgPointerDerefExp(isSgPointerDerefExp(n));
  }
  else if (isSgArrowExp(n)) {
    // if this is a varrefexp and it is not initialized, we flag it.
    // do only if it is by itself or on right hand side of assignment
    cerr << " @@@@@@@@@ DETECTED isSgArrowExp : " << n->unparseToString() << endl;
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
    visit_pointer_movement( n );
  }
  // *********************** Detect pointer movements, e.g ++, -- *********
  
  // *********************** Detect delete (c++ free) *********
  else if( isSgDeleteExp( n )) {
    visit_delete( isSgDeleteExp( n ));
  }
  // *********************** Detect delete (c++ free) *********

  else if (isSgReturnStmt(n)) {
#if 1
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
