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


// ---------------------------------------
 // Perform all transformations...
 //
 // Do insertions LIFO, so, e.g. if we want to add stmt1; stmt2; after stmt0
 // add stmt2 first, then add stmt1
 // ---------------------------------------
void RtedTransformation::executeTransformations() {

 BOOST_FOREACH( SgReturnStmt* ret, returnstmt )
   changeReturnStmt(ret);

 // bracket function calls and scope statements with calls to enterScope and exitScope.
 //
 // Note: For function calls, this must occur before variable
 // initialization, so that assignments of function return values happen before exitScope is called.
 if (RTEDDEBUG()) cerr << "\n # Elements in scopes  : " << scopes.size() << endl;
 BOOST_FOREACH( StatementNodePair i, scopes ) {
   SgStatement* stmt_to_bracket = i.first;
   SgNode* end_of_scope = i.second;
   ROSE_ASSERT( stmt_to_bracket );
   ROSE_ASSERT( end_of_scope );
   // bracket all scopes except constructors with enter/exit
   bracketWithScopeEnterExit( stmt_to_bracket, end_of_scope );
 }

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
 BOOST_FOREACH( SgFunctionDefinition* fndef, function_definitions ) {
   if( isConstructor( fndef -> get_declaration() ))
     bracketWithScopeEnterExit( fndef );

    // tps : 09/14/2009 : handle templates
#if 1
   SgTemplateInstantiationFunctionDecl* istemplate =
     isSgTemplateInstantiationFunctionDecl(fndef->get_parent());
   if (RTEDDEBUG()) cerr <<" ^^^^ Found definition : " << fndef->get_declaration()->get_name().str() <<       "  is template: " << istemplate << endl;
   if (istemplate) {
     SgGlobal* gl = isSgGlobal(istemplate->get_parent());
     ROSE_ASSERT(gl);
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

 std::map<SgVarRefExp*,std::pair<SgInitializedName*,bool> >::const_iterator it5 = variableIsInitialized.begin();
 for (; it5 != variableIsInitialized.end(); it5++) {
   SgVarRefExp* varref = it5->first;
   std::pair<SgInitializedName*,bool> p = it5->second;
   SgInitializedName* init = p.first;
   bool ismalloc = p.second;
   ROSE_ASSERT(varref);
   insertInitializeVariable(init, varref,ismalloc);
 }

 if (RTEDDEBUG()) cerr << "\n # Elements in create_array_define_varRef_multiArray  : "  << create_array_define_varRef_multiArray.size() << endl;
 std::map<SgVarRefExp*, RTedArray*>::const_iterator itm =  create_array_define_varRef_multiArray.begin();
 for (; itm != create_array_define_varRef_multiArray.end(); itm++) {
   SgVarRefExp* array_node = itm->first;
   RTedArray* array_size = itm->second;
   insertArrayCreateCall(array_node, array_size);
 }

 if (RTEDDEBUG()) cerr << "\n # Elements in variable_access_varref  : " << variable_access_varref.size() << endl;
 BOOST_FOREACH( SgVarRefExp* vr, variable_access_varref ) {
   ROSE_ASSERT(vr);
   insertAccessVariable(vr,NULL);
 }

 if (RTEDDEBUG()) cerr << "\n # Elements in variable_access_pointer  : " << variable_access_pointerderef.size() << endl;
 std::map<SgExpression*, SgVarRefExp*>::const_iterator itAccess2 = variable_access_pointerderef.begin();
 for (; itAccess2 != variable_access_pointerderef.end(); itAccess2++) {
   // can be SgVarRefExp or SgPointerDerefExp
   SgExpression* pd = isSgExpression(itAccess2->first);
   if (pd) {
     SgVarRefExp* in = isSgVarRefExp(itAccess2->second);
     insertAccessVariable(in, pd);
   }
 }

 if (RTEDDEBUG()) cerr << "\n # Elements in variable_access_arrowexp  : " << variable_access_arrowexp.size() << endl;
 std::map<SgExpression*, SgVarRefExp*>::const_iterator itAccessArr = variable_access_arrowexp.begin();
 for (; itAccessArr != variable_access_arrowexp.end(); itAccessArr++) {
   SgExpression* pd = isSgExpression(itAccessArr->first);
   if (pd) {
     SgVarRefExp* in = isSgVarRefExp(itAccessArr->second);
     insertAccessVariable(in, pd);
   }
 }

 if (RTEDDEBUG()) cerr << "\n # Elements in variable_access_arrowthisexp  : " << variable_access_arrowthisexp.size() << endl;
 std::map<SgExpression*, SgThisExp*>::const_iterator itAccessArr2 =  variable_access_arrowthisexp.begin();
 for (; itAccessArr2 != variable_access_arrowthisexp.end(); itAccessArr2++) {
   SgExpression* pd = isSgExpression(itAccessArr2->first);
   if (pd) {
       SgThisExp* in = isSgThisExp(itAccessArr2->second);
   	insertCheckIfThisNull(in);
   	insertAccessVariable(in, pd);
   }
 }

 if (RTEDDEBUG()) cerr << "\n # Elements in create_array_define_varRef_multiArray_stack  : "  << create_array_define_varRef_multiArray_stack.size() << endl;
 std::map<SgInitializedName*, RTedArray*>::const_iterator itv = create_array_define_varRef_multiArray_stack.begin();
 for (; itv != create_array_define_varRef_multiArray_stack.end(); itv++) {
   SgInitializedName* array_node = itv->first;
   RTedArray* array_size = itv->second;
   insertArrayCreateCall(array_node, array_size);
 }

 if (RTEDDEBUG()) cerr << "\n # Elements in variable_declarations  : " << variable_declarations.size() << endl;
 BOOST_FOREACH( SgInitializedName* initName, variable_declarations )
   insertVariableCreateCall(initName);

 // make sure register types wind up before variable & array create, so that
 // types are always available
 if (RTEDDEBUG()) cerr << "\n # Elements in class_definitions  : " << class_definitions.size() << endl;
 std::map<SgClassDefinition*,RtedClassDefinition*> ::const_iterator refIt = class_definitions.begin();
 for (; refIt != class_definitions.end(); refIt++) {
   RtedClassDefinition* rtedClass = refIt->second;
   ROSE_ASSERT(rtedClass);
   insertRegisterTypeCall(rtedClass);
   insertCreateObjectCall( rtedClass );
 }

 if (RTEDDEBUG()) cerr << "\n # Elements in create_array_access_call  : " << create_array_access_call.size() << endl;
 std::map<SgExpression*, RTedArray*>::const_iterator ita = create_array_access_call.begin();
 for (; ita != create_array_access_call.end(); ita++) {
   SgExpression* array_node = ita->first;
   RTedArray* array_size = ita->second;
   insertArrayAccessCall(array_node, array_size);
 }

 if (RTEDDEBUG()) cerr << "\n # Elements in function_call_missing_def  : "
   << function_call_missing_def.size() << endl;
 BOOST_FOREACH( SgFunctionCallExp* fncall, function_call_missing_def )
   insertAssertFunctionSignature( fncall );

 if (RTEDDEBUG()) cerr << "\n # Elements in function_definitions  : "
   << function_definitions.size() << endl;
 BOOST_FOREACH( SgFunctionDefinition* fndef, function_definitions) {
   insertVariableCreateInitForParams( fndef );
   insertConfirmFunctionSignature( fndef );
 }

 if (RTEDDEBUG()) cerr << "\n # Elements in funccall_call : " << function_call.size() << endl;
 BOOST_FOREACH( RtedArguments* funcs, function_call) {
   if (isStringModifyingFunctionCall(funcs->f_name) ) {
     //if (RTEDDEBUG()) cerr << " .... Inserting Function Call : " << name << endl;
     insertFuncCall(funcs);
   } else if (isFileIOFunctionCall(funcs->f_name)) {
     insertIOFuncCall(funcs);
   } else if (isFunctionCallOnIgnoreList(funcs->f_name)) {
     // dont do anything.
   }
 }

 BOOST_FOREACH( SgExpression* expr, frees)
   insertFreeCall( expr );

 BOOST_FOREACH( SgFunctionCallExp* fcallexp, reallocs)
	insertReallocateCall( fcallexp );



 if (RTEDDEBUG())  cerr << "Inserting main close call" << endl;
 ROSE_ASSERT(mainLast);
 insertMainCloseCall(mainLast);
}


#endif
