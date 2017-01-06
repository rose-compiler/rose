// A code generator doing template specialization
//
// 
//
// Liao, 9/1/2016

#include <rose.h>

#include <vector>
#include <sstream>
#include <string>

namespace XGEN
{
  //find the template function to be specialized
  std::vector<SgTemplateFunctionDeclaration*> findTargetTemplateFunctions(SgProject* p);

  // generate and return a specialized template function, from an input template func, default scope is the same scope as the original one
  // caller provides the name of the type used for specialize the template function , like struct switcher_exec {};
  SgTemplateFunctionDeclaration* specializeTemplateFunction(SgTemplateFunctionDeclaration* input_func, std::string special_type_name);


  // Find the first declaration of a named namespace, start the search from a given scope for efficiency.
  SgNamespaceDeclarationStatement* findFirstNamespace(std::string name, SgScopeStatement* start_scope);
}

using namespace XGEN;
using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

int main (int argc, char** argv)
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc, argv);

  // Run internal consistency tests on AST
  AstTests::runAllTests(project);

  // Insert your own manipulations of the AST here...
  std::vector<SgTemplateFunctionDeclaration*> templates = findTargetTemplateFunctions (project);
//  ROSE_ASSERT (templates.size() ==1); // TODO remove later
  SgTemplateFunctionDeclaration* tfunc = templates[0];

  // build and insert the specialized function
  specializeTemplateFunction (tfunc, "switcher_exec");

  // Generate source code from AST and invoke your
  // desired backend compiler
  return backend(project);
}

//TODO: move to librose.so
// Find the first declaration of a named namespace, start the search from a given scope for efficiency.
SgNamespaceDeclarationStatement* XGEN::findFirstNamespace(std::string name, SgScopeStatement* start_scope)
{
   ROSE_ASSERT (start_scope != NULL);
   SgNamespaceDeclarationStatement* result = findDeclarationStatement<SgNamespaceDeclarationStatement> (start_scope, name, NULL, false);
   ROSE_ASSERT (result != NULL); 
   return result; 
}

/*
  template <typename LOOP_BODY>
 //    inline __attribute__((always_inline))
     void forall(switcher_exec,
         Index_type begin, Index_type end,
         LOOP_BODY loop_body)                                                                                                
     {                                                                                                                       
       switch(switcher::getRandomPolicy()) {                                                                                 
         case switcher::seq_exec:                                                                                            
           RAJA::forall( RAJA::seq_exec(), begin, end, loop_body ); break;                                                   
         case switcher::omp_parallel_for_exec:                                                                               
           RAJA::forall( RAJA::omp_parallel_for_exec(), begin, end, loop_body ); break;                                      
       }                                                                                                                     
     }
 */
// Generate and insert a specialized template function
// We should insert after the switcher {} namespace so used types are declared first.
// Right now, we insert the new func as the last one in the parent scope of the original template function
SgTemplateFunctionDeclaration* XGEN::specializeTemplateFunction(SgTemplateFunctionDeclaration* input_func, std::string special_type_name)
{
  ROSE_ASSERT (input_func != NULL);
  SgScopeStatement* scope = input_func->get_scope();
  ROSE_ASSERT (scope!= NULL);

  // Create the special type used for specialization
  SgClassDeclaration * s_struct = buildStructDeclaration (special_type_name, scope);
  //insertStatementAfter (input_func, s_struct);
  // Not immediately after the original template function
  // Inserting as the last one to ensure all types are declared in advance.
  appendStatement(s_struct, scope);
  
  SgTemplateType * ttype = buildTemplateType (SgName("LOOP_BODY"));
  SgTemplateParameter * tparameter = buildTemplateParameter (SgTemplateParameter::type_parameter, ttype);

  // switcher_exec, Index_type begin, Index_type end, LOOP_BODY loop_body
  SgInitializedName* iname1 = buildInitializedName (SgName(""), s_struct->get_type());
  SgInitializedName* iname2 = buildInitializedName (SgName("begin"), buildIntType()); //TODO use Index_type instead
  SgInitializedName* iname3 = buildInitializedName (SgName("end"), buildIntType());
  SgInitializedName* iname4 = buildInitializedName (SgName("loop_body"), ttype);
  SgFunctionParameterList * parlist = buildFunctionParameterList (iname1, iname2, iname3, iname4);
  SgTemplateParameterPtrList * tplist = new SgTemplateParameterPtrList();
  tplist->push_back (tparameter);
  SgName sname("forall");
  // nondefining declaration
  SgTemplateFunctionDeclaration* sfunc = buildNondefiningTemplateFunctionDeclaration (sname, buildVoidType(), parlist, scope, NULL,  tplist);
  ROSE_ASSERT (sfunc != NULL);
  // deepCopy (input_func);
  // Defining declaration
  SgTemplateFunctionDeclaration* sfunc2 = buildDefiningTemplateFunctionDeclaration (sname, buildVoidType(), parlist, scope, NULL, sfunc );
  ROSE_ASSERT (sfunc2 != NULL);

  //insert here !!
  insertStatementAfter (s_struct, sfunc2); // (target, new_stmt)

  // Now generate the template function body 
  /*   switch(switcher::getRandomPolicy()) {                                                                                 
         case switcher::seq_exec:                                                                                            
           RAJA::forall( RAJA::seq_exec(), begin, end, loop_body ); break;                                                   
         case switcher::omp_parallel_for_exec:                                                                               
           RAJA::forall( RAJA::omp_parallel_for_exec(), begin, end, loop_body ); break;                                      
  */ 
  SgTemplateFunctionDefinition* func_def = isSgTemplateFunctionDefinition(sfunc2->get_definition());
  ROSE_ASSERT (func_def != NULL);
  SgBasicBlock* func_body = func_def->get_body();
  ROSE_ASSERT (func_body != NULL);

  SgNamespaceDeclarationStatement*  nspace = findFirstNamespace("switcher", scope); 
  // Two definition nodes for one namespace. The global one has the symbol table
  SgNamespaceDefinitionStatement* nspace_def = nspace->get_definition()->get_global_definition();
  ROSE_ASSERT (nspace_def != NULL);

  // get the first function with name matched
  SgFunctionSymbol * fsym = nspace_def->lookup_function_symbol (SgName("getRandomPolicy"));
  ROSE_ASSERT (fsym != NULL);
  SgFunctionCallExp* func_exp = buildFunctionCallExp(fsym);
  SgBasicBlock* sbody = buildBasicBlock(); // switch body
  SgSwitchStatement* switch_stmt = buildSwitchStatement(func_exp, sbody);
  appendStatement(switch_stmt, func_body);

  //loop, iterate over the enum symbols
  // case switcher::seq_exec:                                                                                            
  //    RAJA::forall( RAJA::seq_exec(), begin, end, loop_body ); break; 
  SgEnumDeclaration*  enum_decl= findDeclarationStatement<SgEnumDeclaration> (scope, "POLICY_TYPE", NULL, true); // obtain the defining one
  ROSE_ASSERT (enum_decl!=NULL);

  // Obtain the namespace enclosing RAJA::forall()
  SgNamespaceDeclarationStatement*  nspace_raja = findFirstNamespace("RAJA", getGlobalScope(input_func)); // the input template function is part of RAJA namespace
  ROSE_ASSERT (nspace_raja);
  SgNamespaceDefinitionStatement* nspace_raja_def = nspace_raja->get_definition()->get_global_definition();
  ROSE_ASSERT (nspace_raja_def != NULL); // this one contain the symbol table for forall() functions
//  cout<<"debug:"<< endl;
//  nspace_raja->get_file_info()->display();

  SgInitializedNamePtrList name_list = enum_decl->get_enumerators();
  for (SgInitializedNamePtrList::iterator iter = name_list.begin(); iter != name_list.end() -1; iter++) // -1 to skip the last field
  {
    SgInitializedName* iname = *iter; 
    ROSE_ASSERT (iname != NULL);

    SgEnumFieldSymbol * esym = isSgEnumFieldSymbol(iname-> search_for_symbol_from_symbol_table());
    ROSE_ASSERT (esym != NULL);
    SgName ename = esym->get_name(); // seq_exec, etc.

    SgBasicBlock * case_body = buildBasicBlock();
    SgCaseOptionStmt* case_stmt = buildCaseOptionStmt ( buildEnumVal(esym), case_body);
    //add to the switch body, doing this early
    appendStatement (case_stmt, sbody);

    // RAJA::forall( RAJA::seq_exec(), begin, end, loop_body );
    // policy class declaration: seq_exec, etc
    SgClassDeclaration* pclass_decl = findDeclarationStatement<SgClassDeclaration> ( scope, ename.getString(), NULL, false);
    ROSE_ASSERT (pclass_decl);
    SgType* pclass_type = pclass_decl->get_type();
    ROSE_ASSERT (pclass_type);
    // the input template function's parameter types are reused, just prepend a new policy class type here! 
    SgFunctionParameterTypeList* specialized_type_list = buildFunctionParameterTypeList(pclass_type);
    //  cout<< "debug: appending first func arg type:"<< pclass_type->class_name() << " "<< pclass_type->get_mangled()<<endl;
    SgFunctionParameterTypeList* origin_type_list = input_func->get_type()->get_argument_list();
    SgTypePtrList tplist = origin_type_list->get_arguments();
    for (SgTypePtrList::iterator iter1= tplist.begin(); iter1 != tplist.end(); iter1++)
    {
      SgType* ctype = *iter1; 
      //debug
      //cout<< "debug: appending input func arg type:"<< ctype->class_name()<< " "<< ctype->get_mangled()<<endl;
      specialized_type_list->append_argument(ctype);
    }

    SgFunctionType* func_type = buildFunctionType (buildVoidType(), specialized_type_list ); 
    // I have to develop my own search !! 
    SgFunctionSymbol* forall_sym = nspace_raja_def->lookup_function_symbol(SgName("forall"), func_type);
    ROSE_ASSERT (forall_sym != NULL);
    
    // build the function call RAJA::forall( RAJA::seq_exec(), begin, end, loop_body );
    SgConstructorInitializer* cst = buildConstructorInitializer (NULL, buildExprListExp(), pclass_decl->get_type(),true, false, true, false);
    SgExprListExp* parameters = buildExprListExp (cst, buildVarRefExp("begin", sbody), buildVarRefExp("end" ,sbody), buildVarRefExp("loop_body" ,sbody));
    SgFunctionCallExp* call_exp = buildFunctionCallExp (forall_sym, parameters);
    SgExprStatement* call_stmt = buildExprStatement (call_exp);
    appendStatement(call_stmt, case_body);
    // break;
    appendStatement(buildBreakStmt(), case_body);

//    cout<<"debug constructor initializer"<<endl;
//    cout<< cst->unparseToString()<<endl;
  } // end of the loop

#if 0
  //debugging 
  cout<<"debugging here ..."<<endl;
   cout<<sfunc2->unparseToString()<<endl;
  // body should be unparsable. 
  cout<<func_body->unparseToString()<<endl;
#endif  
  return sfunc2;
}

// For now, we find template functions with #pragma xgen specialize_template
std::vector<SgTemplateFunctionDeclaration*> XGEN::findTargetTemplateFunctions(SgProject* project)
{
  std::vector<SgTemplateFunctionDeclaration*> functions; 
  Rose_STL_Container <SgNode*> testList = NodeQuery::querySubTree (project, V_SgPragmaDeclaration);
  for (Rose_STL_Container <SgNode*>::iterator iter = testList.begin(); iter!= testList.end(); iter++)
  {
    SgPragmaDeclaration* pragma= isSgPragmaDeclaration(*iter);
    ROSE_ASSERT (pragma!= NULL);
    string pragmaString = pragma->get_pragma()->get_pragma();
    istringstream istr(pragmaString);
    std::string key;
    istr >> key;
    if (key == "xgen")
    {
      istr >> key;
      if (key == "specialize_template")
      {
        SgStatement* n_stmt = getNextStatement (pragma);
        ROSE_ASSERT (n_stmt != NULL);
        SgTemplateFunctionDeclaration* func = isSgTemplateFunctionDeclaration (n_stmt);
        ROSE_ASSERT (func != NULL);
        functions.push_back(func);
      }
    } 
  } // end for
  return functions;
}
