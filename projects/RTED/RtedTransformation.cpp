#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <set>

#include <boost/foreach.hpp>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"
//#include "RuntimeSystem.h"

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
 * Perform all transformations needed (Step 2)
 * -----------------------------------------------------------*/
void RtedTransformation::transform(SgProject* project, set<string> &rtedfiles) {
  cout << "Running Transformation..." << endl;
  globalScope = getFirstGlobalScope(isSgProject(project));

  ROSE_ASSERT( project);

  this -> rtedfiles = &rtedfiles;

  // traverse the AST and find locations that need to be transformed
  symbols->traverse(project, preorder);
  roseCreateHeap = symbols->roseCreateHeap;
  roseAccessHeap = symbols->roseAccessHeap;
  roseFunctionCall = symbols->roseFunctionCall;
  roseAssertFunctionSignature = symbols->roseAssertFunctionSignature;
  roseConfirmFunctionSignature = symbols->roseConfirmFunctionSignature;
  roseFreeMemory = symbols->roseFreeMemory;
  roseReallocateMemory = symbols->roseReallocateMemory;
  roseCheckpoint = symbols->roseCheckpoint;
  roseConvertIntToString=symbols->roseConvertIntToString;
  roseCreateVariable = symbols->roseCreateVariable;
  roseCreateObject = symbols->roseCreateObject;
  roseInitVariable = symbols->roseInitVariable;
  roseMovePointer = symbols->roseMovePointer;
  roseAccessVariable = symbols->roseAccessVariable;
  roseEnterScope = symbols->roseEnterScope;
  roseExitScope = symbols->roseExitScope;
  roseIOFunctionCall = symbols->roseIOFunctionCall;
  roseRegisterTypeCall = symbols->roseRegisterTypeCall;
  size_t_member = symbols->size_t_member;
  roseCheckIfThisNULL = symbols->roseCheckIfThisNULL;

  ROSE_ASSERT(roseCreateHeap);
  ROSE_ASSERT(roseAccessHeap);
  ROSE_ASSERT(roseFunctionCall);
  ROSE_ASSERT(roseAssertFunctionSignature);
  ROSE_ASSERT(roseConfirmFunctionSignature);
  ROSE_ASSERT(roseFreeMemory);
  ROSE_ASSERT(roseReallocateMemory);
  ROSE_ASSERT(roseConvertIntToString);
  ROSE_ASSERT(roseCheckpoint);
  ROSE_ASSERT(roseCreateVariable);
  ROSE_ASSERT(roseCreateObject);
  ROSE_ASSERT(roseInitVariable);
  ROSE_ASSERT(roseMovePointer);
  ROSE_ASSERT(roseAccessVariable);
  ROSE_ASSERT(roseEnterScope);
  ROSE_ASSERT(roseExitScope);
  ROSE_ASSERT(roseIOFunctionCall);
  ROSE_ASSERT(roseRegisterTypeCall);
  ROSE_ASSERT(size_t_member);
  ROSE_ASSERT(roseCheckIfThisNULL);


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
    	  cerr << " **** Inserting file into sourceFileRoseNamespaceMap:" << sf -> get_file_info() -> get_filename() << endl;
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
  //vector<SgNode*>::const_reverse_iterator classIt = results.rbegin();
  //  for (;classIt!=results.rend();classIt++) {
  vector<SgNode*>::const_iterator classIt = resultsInv.begin();
  for (;classIt!=resultsInv.end();classIt++) {
    SgClassDeclaration* classDecl = isSgClassDeclaration(*classIt);
    if (classDecl->get_definingDeclaration()==classDecl)
      if (//classDecl->get_class_type()==SgClassDeclaration::e_class &&
	  !classDecl->get_file_info()->isCompilerGenerated()
	  ) {
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
	  cerr << "\n ** Deep copy: Found classDecl : " << classDecl->get_name().str() << "  in File: " << filename <<
	    "    with number of datamembers: " << vec.size() << "   defining " <<
	    (classDecl->get_definingDeclaration()==classDecl) << endl;
	  if (hasPrivateDataMembers(classDecl)) {
	    //SgClassDeclaration* cdl = instrumentClassDeclarationIntoTopOfAllSourceFiles(project, classDecl);
	    //traverseClasses.push_back(cdl);
	  } //else
	    traverseClasses.push_back(classDecl);
	} else if (    	filename.find("include-staging")==string::npos ||
			filename.find("/usr/include")==string::npos) {
	  std::string classname = classDecl->get_name().str();
#if 0
	  cerr <<"Found a class that is instantiated through the header file" << 
	    "  " << filename << "  class:" << classname << 
	    "  classtype: " << classDecl->class_name() << endl;
	  bool isBasicFile = false;
	  if (classname.compare("__file < char >"))
	    isBasicFile=true;
	  cerr <<"isBasicFile: " << isBasicFile<< "  isCompilerGen:" <<
	    classDecl->get_file_info()->isCompilerGenerated() << endl;
	  //tps : fixme in order to make cxx_stream test work.
	  //if (isBasicFile) 
	  //  instrumentClassDeclarationIntoTopOfAllSourceFiles(project, classDecl);
#endif 	  
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





  // ---------------------------------------
  // Perform all transformations...
  //
  // Do insertions LIFO, so, e.g. if we want to add stmt1; stmt2; after stmt0
  // add stmt2 first, then add stmt1
  // ---------------------------------------


  cerr << "\n Number of Elements in returnstmt  : "
       << returnstmt.size() << endl;
  std::vector<SgReturnStmt*>::const_iterator rita = returnstmt.begin();
  for (; rita != returnstmt.end(); rita++) {
    SgReturnStmt* ret = *rita;
     changeReturnStmt(ret);
  }


  // bracket function calls and scope statements with calls to enterScope and
  // exitScope.  
  //
  // Note: For function calls, this must occur before variable
  // initialization, so that assignments of function return values happen before
  // exitScope is called.
  cerr << "\n Number of Elements in scopes  : "
       << scopes.size() << endl;
  BOOST_FOREACH( StatementNodePair i, scopes ) {
    SgStatement* stmt_to_bracket = i.first;
    SgNode* end_of_scope = i.second;

    ROSE_ASSERT( stmt_to_bracket );
    ROSE_ASSERT( end_of_scope );

    // bracket all scopes except constructors with enter/exit
    bracketWithScopeEnterExit( stmt_to_bracket, end_of_scope );
  }
  BOOST_FOREACH( SgFunctionDefinition* fndef, function_definitions ) {
    // bracket the bodies of constructors with enter/exit.  This is easier than
    // bracketing the variable declarations, and isn't harmful because the
    // return type is fixed.  However, it would not be wrong to simply bracket
    // the variable declaration, e.g.
    //    MyClassWithConstructor a;
    //
    //  transformed to:
    //    enterScope("constructor");
    //    MyClassWithConstructor a;
    //    exitScope("constructor");
    //
    if( isConstructor( fndef -> get_declaration() )) 
      bracketWithScopeEnterExit( fndef );

     // tps : 09/14/2009 : handle templates
#if 1
    SgTemplateInstantiationFunctionDecl* istemplate = 
      isSgTemplateInstantiationFunctionDecl(fndef->get_parent());
    cerr <<" ^^^^ Found definition : " << fndef->get_declaration()->get_name().str() << 
      "  is template: " << istemplate << endl;
    if (istemplate) {
      SgGlobal* gl = isSgGlobal(istemplate->get_parent());
      ROSE_ASSERT(gl);
      //SgTemplateInstantiationFunctionDecl* cfunc =
      //isSgTemplateInstantiationFunctionDecl(deepCopyNode(istemplate));
      //ROSE_ASSERT(cfunc);
      vector<SgNode*> nodes2 = NodeQuery::querySubTree(istemplate, V_SgLocatedNode);
      vector<SgNode*>::const_iterator nodesIT2 = nodes2.begin();
      for (; nodesIT2 != nodes2.end(); nodesIT2++) {
    	  SgLocatedNode* node = isSgLocatedNode(*nodesIT2);
    	  ROSE_ASSERT(node);
    	  Sg_File_Info* file_info = node->get_file_info();
    	  file_info->setOutputInCodeGeneration();
      }
      // insert after template declaration
      // find last template declaration, which should be part of SgGlobal
      vector<SgNode*> nodes3 = NodeQuery::querySubTree(gl, V_SgTemplateInstantiationFunctionDecl);
      vector<SgNode*>::const_iterator nodesIT3 = nodes3.begin();
      SgTemplateInstantiationFunctionDecl* templ = NULL;
      for (; nodesIT3 != nodes3.end(); nodesIT3++) {
    	  SgTemplateInstantiationFunctionDecl* temp = isSgTemplateInstantiationFunctionDecl(*nodesIT3);
    	  string temp_str = temp->get_qualified_name().str();
    	  string template_str = istemplate->get_qualified_name().str();
    	  if (temp!=istemplate)
    		  if (temp_str.compare(template_str)==0)
    		  templ =temp;
//    	  cerr << " Found templ : " << temp->get_qualified_name().str() << ":"<<temp<<
//			  "  istemplate : " << istemplate->get_qualified_name().str() << ":"<<istemplate<<
//			  "         " << (temp_str.compare(template_str)==0) << endl;
      }
      ROSE_ASSERT(templ);
      SageInterface::removeStatement(istemplate);
      SageInterface::insertStatementAfter(templ,istemplate);
    }

#endif
  }
  // add calls to register pointer change after pointer arithmetic
  BOOST_FOREACH( SgExpression* op, pointer_movements ) {
    ROSE_ASSERT( op );
    insert_pointer_change( op );
  }


#if 0
  // before we insert the intitialized variables,
  // we need to insert the temporary statements that
  // we found during our traversal
  cerr
    << "\n Number of Elements in variableIsInitialized  : "
    << variableIsInitialized.size() << endl;
  std::map<SgStatement*,SgStatement*>::const_iterator itStmt =
    insertThisStatementLater.begin();
  for (; itStmt != insertThisStatementLater.end(); itStmt++) {
    SgStatement* newS = itStmt->first;
    SgStatement* old = itStmt->second;
    insertStatementAfter(old,newS);
  }
#endif


  std::map<SgVarRefExp*,std::pair<SgInitializedName*,bool> >::const_iterator it5 =
    variableIsInitialized.begin();
  for (; it5 != variableIsInitialized.end(); it5++) {
    SgVarRefExp* varref = it5->first;
    std::pair<SgInitializedName*,bool> p = it5->second;
    SgInitializedName* init = p.first;
    bool ismalloc = p.second;
    ROSE_ASSERT(varref);
    //cerr << "      varInit : " << varref->unparseToString() <<
    //  "    malloc: " << ismalloc << endl;
    insertInitializeVariable(init, varref,ismalloc);

#if 0
    // handle special case of assigninitializer
	SgType* thetype = init->get_type();
	if (isSgPointerType(thetype) && ismalloc) {
		//cerr << "$$$$$ Found the AssignInitializer : " << isSgVarRefExp(varref)->get_parent() << endl;
		//cerr << "$$$$$ Found the InitName : " << init->unparseToString() << endl;
		//cerr << "$$$$$ Found the InitName Type: " << thetype->class_name() << endl;
		SgExpression* tmpExp = buildPointerDerefExp(varref);
	    insertInitializeVariable(init, tmpExp,ismalloc);
	    insertVariableCreateCall(init, tmpExp);
		// insertArrayCreateCall(tmpExp, array_size);
	}
#endif
  }

  cerr << "\n Number of Elements in create_array_define_varRef_multiArray  : "
       << create_array_define_varRef_multiArray.size() << endl;
  std::map<SgVarRefExp*, RTedArray*>::const_iterator itm =
    create_array_define_varRef_multiArray.begin();
  for (; itm != create_array_define_varRef_multiArray.end(); itm++) {
    SgVarRefExp* array_node = itm->first;
    RTedArray* array_size = itm->second;
    //cerr << ">>> INserting array create (VARREF): "
    //		<< array_node->unparseToString() << "  size : "
    //		<< array_size->unparseToString() << endl;
    insertArrayCreateCall(array_node, array_size);
  }

  cerr << "\n Number of Elements in variable_access_varref  : "
       << variable_access_varref.size() << endl;
  std::vector<SgVarRefExp*>::const_iterator itAccess =
    variable_access_varref.begin();
  for (; itAccess != variable_access_varref.end(); itAccess++) {
    // can be SgVarRefExp or SgPointerDerefExp
    SgNode* node = *itAccess;
    SgVarRefExp* vr = isSgVarRefExp(node);
    ROSE_ASSERT(vr);
    insertAccessVariable(vr,NULL);
  }

  cerr << "\n Number of Elements in variable_access_pointer  : "
       << variable_access_pointerderef.size() << endl;
  std::map<SgExpression*, SgVarRefExp*>::const_iterator itAccess2 =
    variable_access_pointerderef.begin();
  for (; itAccess2 != variable_access_pointerderef.end(); itAccess2++) {
    // can be SgVarRefExp or SgPointerDerefExp
    SgExpression* pd = isSgExpression(itAccess2->first);
    SgVarRefExp* in = isSgVarRefExp(itAccess2->second);
    if (pd)
      insertAccessVariable(in, pd);
  }

  cerr << "\n Number of Elements in variable_access_arrowexp  : "
       << variable_access_arrowexp.size() << endl;
  std::map<SgExpression*, SgVarRefExp*>::const_iterator itAccessArr =
    variable_access_arrowexp.begin();
  for (; itAccessArr != variable_access_arrowexp.end(); itAccessArr++) {
    SgExpression* pd = isSgExpression(itAccessArr->first);
    SgVarRefExp* in = isSgVarRefExp(itAccessArr->second);
    if (pd)
      insertAccessVariable(in, pd);
  }

  cerr << "\n Number of Elements in variable_access_arrowthisexp  : "
       << variable_access_arrowthisexp.size() << endl;
  std::map<SgExpression*, SgThisExp*>::const_iterator itAccessArr2 =
    variable_access_arrowthisexp.begin();
  for (; itAccessArr2 != variable_access_arrowthisexp.end(); itAccessArr2++) {
    SgExpression* pd = isSgExpression(itAccessArr2->first);
    SgThisExp* in = isSgThisExp(itAccessArr2->second);
    if (pd) {
    	insertCheckIfThisNull(in);
      insertAccessVariable(in, pd);
    }
  }


  cerr
    << "\n Number of Elements in create_array_define_varRef_multiArray_stack  : "
    << create_array_define_varRef_multiArray_stack.size() << endl;
  std::map<SgInitializedName*, RTedArray*>::const_iterator itv =
    create_array_define_varRef_multiArray_stack.begin();
  for (; itv != create_array_define_varRef_multiArray_stack.end(); itv++) {
    SgInitializedName* array_node = itv->first;
    RTedArray* array_size = itv->second;
    //cerr << ">>> INserting array create (VARREF): "
    //		<< array_node->unparseToString() << "  size : "
    //		<< array_size->unparseToString() << endl;
    insertArrayCreateCall(array_node, array_size);
  }

  cerr << "\n Number of Elements in variable_declarations  : "
       << variable_declarations.size() << endl;
  std::vector<SgInitializedName*>::const_iterator it1 =
    variable_declarations.begin();
  for (; it1 != variable_declarations.end(); it1++) {
    SgInitializedName* node = *it1;
    insertVariableCreateCall(node);
  }

  // make sure register types wind up before variable & array create, so that
  // types are always available
  cerr
    << "\n Number of Elements in class_definitions  : "
    << class_definitions.size() << endl;
  std::map<SgClassDefinition*,RtedClassDefinition*> ::const_iterator refIt =
    class_definitions.begin();
  for (; refIt != class_definitions.end(); refIt++) {
    RtedClassDefinition* rtedClass = refIt->second;
    ROSE_ASSERT(rtedClass);
    insertRegisterTypeCall(rtedClass);

    insertCreateObjectCall( rtedClass );
  }

  cerr << "\n Number of Elements in create_array_access_call  : "
       << create_array_access_call.size() << endl;
  std::map<SgExpression*, RTedArray*>::const_iterator ita =
    create_array_access_call.begin();
  for (; ita != create_array_access_call.end(); ita++) {
    SgExpression* array_node = ita->first;
    RTedArray* array_size = ita->second;
    insertArrayAccessCall(array_node, array_size);
  }



  cerr
    << "\n Number of Elements in function_call_missing_def  : "
    << function_call_missing_def.size() << endl;
  BOOST_FOREACH( SgFunctionCallExp* fncall, function_call_missing_def ) {
    insertAssertFunctionSignature( fncall );
  }


  cerr
    << "\n Number of Elements in function_definitions  : "
    << function_definitions.size() << endl;
  BOOST_FOREACH( SgFunctionDefinition* fndef, function_definitions) {
    insertVariableCreateInitForParams( fndef );
    insertConfirmFunctionSignature( fndef );
  }

  cerr << "\n Number of Elements in funccall_call  : "
       << function_call.size() << endl;
  std::vector<RtedArguments*>::const_iterator it4 =
    function_call.begin();
  for (; it4 != function_call.end(); it4++) {
    RtedArguments* funcs = *it4;
    if (isStringModifyingFunctionCall(funcs->f_name) ) {
      //cerr << " .... Inserting Function Call : " << name << endl;
      insertFuncCall(funcs);
    } else if (isFileIOFunctionCall(funcs->f_name)) {
      insertIOFuncCall(funcs);
    } else if (isFunctionCallOnIgnoreList(funcs->f_name)) {
      // dont do anything.
    } else {
      // add other internal function calls, such as push variable on stack
      //insertStackCall(funcs);
    }
  }
  cerr << "\n Number of Elements in frees  : " << frees.size() << endl;
  std::vector< SgExpression* >::const_iterator it_frees = frees.begin();
  for (; it_frees != frees.end(); it_frees++) {
    insertFreeCall( *it_frees );
  }

  cerr << "\n Number of Elements in reallocs  : " << reallocs.size() << endl;
  std::vector< SgFunctionCallExp* >::const_iterator it_reallocs = reallocs.begin();
  for (; it_reallocs != reallocs.end(); it_reallocs++) {
    insertReallocateCall( *it_reallocs );
  }



  //ROSE_ASSERT(globalScope);
  //appendGlobalConstructor(globalScope);
  cerr << "Inserting main close call" << endl;
  // insert main call to ->close();
  ROSE_ASSERT(mainLast);
  insertMainCloseCall(mainLast);
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
