#include <rose.h>
#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;



/*
//instrumented calls for all defined structs/classes:
RTSI_registerType("A", //name of class
		  sizeof(A),
		  "b", "SgInt", offsetof(A,b)
		  "c", "SgChar",offsetof(A,c) ); //for each member the name ,type and offset

RTSI_registerType("B", sizeof(B), 
		  "instanceOfA", "A", offsetof(B,instanceOfA),
		  "d", "SgDouble", offsetof(B,d) );
*/


void RtedTransformation::insertRegisterTypeCall(SgInitializedName* initName
						  ) {

  SgStatement* stmt = getSurroundingStatement(initName);
  // make sure there is no extern in front of stmt
  bool externQual = isGlobalExternVariable(stmt);
  if (externQual) {
    cerr << "Skipping this insertVariableCreateCall because it probably occurs multiple times (with and without extern)." << endl;
    return;
  }


  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    string name = initName->get_mangled_name().str();

    ROSE_ASSERT(scope);
    // what if there is an array creation within a ClassDefinition
    if ( isSgClassDefinition(scope)) {
      // new stmt = the classdef scope
      SgClassDeclaration* decl = isSgClassDeclaration(scope->get_parent());
      ROSE_ASSERT(decl);
      stmt = isSgVariableDeclaration(decl->get_parent());
      if (!stmt) {
	cerr << " Error . stmt is unknown : " << decl->get_parent()->class_name() << endl;
	exit(1);
      }
      scope = scope->get_scope();
      // We want to insert the stmt before this classdefinition, if its still in a valid block
      cerr <<" ....... Found ClassDefinition Scope. New Scope is : " << scope->class_name() << "  stmt:" << stmt->class_name() <<endl;
    }
    // what is there is an array creation in a global scope
    else if (isSgGlobal(scope)) {
      cerr <<"RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << endl;
      // We need to add this new statement to the beginning of main
      // get the first statement in main as stmt
      stmt = mainFirst;
      scope = stmt->get_scope();

      SgExprStatement* exprStmt = buildVariableCreateCallStmt( initName, stmt);
      // kind of hackey.  Really we should be prepending into main's body, not
      // inserting relative its first statement.
      //insertStatementBefore(isSgStatement(stmt), exprStmt);
    }
    if (isSgBasicBlock(scope)) {
      // insert new stmt (exprStmt) after (old) stmt
      SgExprStatement* exprStmt = buildVariableCreateCallStmt( initName, stmt);
#if 1
      cerr << "++++++++++++ stmt :"<<stmt << " mainFirst:"<<mainFirst<<
      "   initName->get_scope():"<<initName->get_scope() <<
      "   mainFirst->get_scope():"<<mainFirst->get_scope()<<endl;
      if( stmt == mainFirst && initName->get_scope()!=mainFirst->get_scope()) {
        insertStatementBefore(isSgStatement(stmt), exprStmt);
        cerr << "+++++++ insert Before... "<<endl;
      } else {
        // insert new stmt (exprStmt) after (old) stmt
        insertStatementAfter(isSgStatement(stmt), exprStmt);
        cerr << "+++++++ insert After... "<<endl;
      }
#endif
    }
    else if (isSgNamespaceDefinitionStatement(scope)) {
      cerr <<"RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << endl;
    } else {
      cerr
	<< "RuntimeInstrumentation :: Surrounding Block is not Block! : "
	<< name << " : " << scope->class_name() << endl;
      ROSE_ASSERT(false);
    }
  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << stmt->class_name() << endl;
    ROSE_ASSERT(false);
  }

}
