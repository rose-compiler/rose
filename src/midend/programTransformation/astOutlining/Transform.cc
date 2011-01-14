/**
 *  \file Transform/Transform.cc
 *
 *  \brief Implements the outlining transformation.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include "astPostProcessing.h"
#include <iostream>
#include <list>
#include <string>

#include "Outliner.hh"
#include "ASTtools.hh"
#include "PreprocessingInfo.hh"
#include "StmtRewrite.hh"
#include "OmpAttribute.h" //regenerate pragma from omp attribute
// =====================================================================

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;
// =====================================================================
SgClassDeclaration* Outliner::generateParameterStructureDeclaration(
                               SgBasicBlock* s, // the outlining target
                               const std::string& func_name_str, // the name for the outlined function, we generate the name of struct based on this.
                               const ASTtools::VarSymSet_t& syms, // variables to be passed as parameters
                               ASTtools::VarSymSet_t& pdSyms, // variables must use pointer types
                               SgScopeStatement* func_scope ) // the scope of the outlined function, could be different from s's global scope
{
  SgClassDeclaration* result = NULL;
  // no need to generate the declaration if no variables are to be passed
  if (syms.empty()) 
    return result;

  ROSE_ASSERT (s != NULL);
  ROSE_ASSERT (func_scope != NULL);
  // this declaration will later on be inserted right before the outlining target calling the outlined function
  ROSE_ASSERT (isSgGlobal(func_scope) != NULL);
  string decl_name = func_name_str+"_data";
  
  result = buildStructDeclaration(decl_name, getGlobalScope(s));
//  result ->setForward(); // cannot do this!! it becomes prototype
//  if (result->get_firstNondefiningDeclaration()  )
//   ROSE_ASSERT(isSgClassDeclaration(result->get_firstNondefiningDeclaration())->isForward() == true);
//   cout<<"Debug Outliner::generateParameterStructureDeclaration(): struct address ="<<result <<" firstNondefining address ="<<result->get_firstNondefiningDeclaration()<<endl;

  // insert member variable declarations to it
  SgClassDefinition *def = result->get_definition();
  ROSE_ASSERT (def != NULL); 
  SgScopeStatement* def_scope = isSgScopeStatement (def);
  ROSE_ASSERT (def_scope != NULL); 
  for (ASTtools::VarSymSet_t::const_iterator i = syms.begin ();
      i != syms.end (); ++i)
  { 
    const SgInitializedName* i_name = (*i)->get_declaration ();
    ROSE_ASSERT (i_name);
    const SgVariableSymbol* i_symbol  = isSgVariableSymbol(i_name->get_symbol_from_symbol_table ());
    ROSE_ASSERT (i_symbol != NULL);
    string member_name= i_name->get_name ().str ();
    SgType* member_type = i_name->get_type() ;
    // use pointer type or its original type?
    if (pdSyms.find(i_symbol) != pdSyms.end())
    {
       member_name = member_name+"_p";
       // member_type = buildPointerType(member_type);
       // Liao, 10/26/2009
       // We use void* instead of type* to ease the handling of C++ class pointers wrapped into the data structure
       // Using void* can avoid adding a forward class declaration  which is needed for classA *
       // It also simplifies unparsing: unparsing the use of classA* has some complications. 
       // The downside is that type casting is needed for setting and using the pointer typed values
       member_type = buildPointerType(buildVoidType());
    }

    SgVariableDeclaration *member_decl = buildVariableDeclaration(member_name, member_type, NULL, def_scope);
    appendStatement (member_decl, def_scope);
  }

  // insert it before the s, but must be in a global scope
  // s might be within a class, namespace, etc. we need to find its ancestor scope
  SgNode* global_scoped_ancestor = getEnclosingFunctionDefinition(s,false); 
  while (!isSgGlobal(global_scoped_ancestor->get_parent())) 
 // use get_parent() instead of get_scope() since a function definition node's scope is global while its parent is its function declaration
  {
    global_scoped_ancestor = global_scoped_ancestor->get_parent();
   }
//  cout<<"global_scoped_ancestor class_name: "<<global_scoped_ancestor->class_name()<<endl; 
  ROSE_ASSERT (isSgStatement(global_scoped_ancestor));
  insertStatementBefore(isSgStatement(global_scoped_ancestor), result); 
  moveUpPreprocessingInfo(result,isSgStatement(global_scoped_ancestor));

  if (global_scoped_ancestor->get_parent() != func_scope )
  {
  //TODO 
   cout<<"Outliner::generateParameterStructureDeclaration() separated file case is not yet handled."<<endl;
   ROSE_ASSERT (false);
  }
  return result;
}

/**
 * Major work of outlining is done here
 *  Preparations: variable collection
 *  Generate outlined function
 *  Replace outlining target with a function call
 *  Append dependent declarations,headers to new file if needed
 */
Outliner::Result
Outliner::outlineBlock (SgBasicBlock* s, const string& func_name_str)
{
  //---------step 1. Preparations-----------------------------------
  //new file, cut preprocessing information, collect variables
  // Generate a new source file for the outlined function, if requested
  SgSourceFile* new_file = NULL;
  if (Outliner::useNewFile)
    new_file = generateNewSourceFile(s,func_name_str);

  // Save some preprocessing information for later restoration. 
  AttachedPreprocessingInfoType ppi_before, ppi_after;
  ASTtools::cutPreprocInfo (s, PreprocessingInfo::before, ppi_before);
  ASTtools::cutPreprocInfo (s, PreprocessingInfo::after, ppi_after);

  // Determine variables to be passed to outlined routine.
  // Also collect symbols which must use pointer dereferencing if replaced during outlining
  ASTtools::VarSymSet_t syms, psyms, pdSyms;
  collectVars (s, syms, psyms);

  std::set<SgInitializedName*> readOnlyVars;

  //Determine variables to be replaced by temp copy or pointer dereferencing.
  if (Outliner::temp_variable|| Outliner::enable_classic || Outliner::useStructureWrapper)
  {
    SageInterface::collectReadOnlyVariables(s,readOnlyVars);
#if 0    
    std::set<SgVarRefExp* > varRefSetB;
    ASTtools::collectVarRefsUsingAddress(s,varRefSetB);
    ASTtools::collectVarRefsOfTypeWithoutAssignmentSupport(s,varRefSetB);
#endif
    // Collect use by address plus non-assignable variables
    // They must be passed by reference if they need to be passed as parameters
    ASTtools::collectPointerDereferencingVarSyms(s,pdSyms);
  }

  // Insert outlined function.
  // grab target scope first
  SgGlobal* glob_scope = const_cast<SgGlobal *> (TransformationSupport::getGlobalScope (s));

  SgGlobal* src_scope = glob_scope;
  if (Outliner::useNewFile)  // change scope to the one within the new source file
  {
    glob_scope = new_file->get_globalScope();
  }

  //-------Step 2. Generate outlined function------------------------------------
  // Generate a structure declaration if useStructureWrapper is set
  // A variable of the struct type will later used to wrap function parameters
  SgClassDeclaration* struct_decl = NULL;
  if (Outliner::useStructureWrapper) 
  {
    struct_decl = generateParameterStructureDeclaration (s, func_name_str, syms, pdSyms, glob_scope);
    ROSE_ASSERT (struct_decl != NULL);
  }

  // generate the function and its prototypes if necessary
  //  printf ("In Outliner::Transform::outlineBlock() function name to build: func_name_str = %s \n",func_name_str.c_str());
  SgFunctionDeclaration* func = generateFunction (s, func_name_str, syms, pdSyms, psyms, struct_decl, glob_scope);
  ROSE_ASSERT (func != NULL);
  ROSE_ASSERT(glob_scope->lookup_function_symbol(func->get_name()));

  // DQ (2/26/2009): At this point "s" has been reduced to an empty block.
  ROSE_ASSERT(s->get_statements().empty() == true);

  // Retest this...
  ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());
#if 0
  printf ("After resetting the parent: func->get_definition() = %p func->get_definition()->get_body()->get_parent() = %p \n",func->get_definition(),func->get_definition()->get_body()->get_parent());
#endif

#if 0
  // DQ (2/24/2009): I think that at this point we should delete the subtree represented by "s"
  // But it might have made more sense to not do a deep copy on "s" in the first place.
  // Why is there a deep copy on "s"?
  SageInterface::deleteAST(s);
#endif

  // Retest this...
  ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());

  //-----------Step 3. Insert the outlined function -------------
  // DQ (2/16/2009): Added (with Liao) the target block which the outlined function will replace.
  // Insert the function and its prototype as necessary  
  ROSE_ASSERT(glob_scope->lookup_function_symbol(func->get_name()));
  insert (func, glob_scope, s); //Outliner::insert() 
  ROSE_ASSERT(glob_scope->lookup_function_symbol(func->get_name()));
  //
  // Retest this...
  ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());

  // reproduce the lost OpenMP pragma attached to a outlining target loop 
  // The assumption is that OmpAttribute is attached to both the pragma and the affected loop
  // in the frontend already.
  // Liao, 3/12/2009
  Rose_STL_Container <SgNode*>  loops = NodeQuery::querySubTree(func,V_SgForStatement);
  if (loops.size()>0)
  {
    Rose_STL_Container <SgNode*>::iterator liter =loops.begin();
    SgForStatement* firstloop = isSgForStatement(*liter); 
    OmpSupport::generatePragmaFromOmpAttribute(firstloop);
  }

  //-----------Step 4. Replace the outlining target with a function call-------------

  // Prepare the parameter of the function call,
  // Generate packing statements, insert them into the beginning of the target s
  std::string wrapper_name;
  // two ways to pack parameters: an array of pointers v.s. A structure
  if (useParameterWrapper || useStructureWrapper)
    wrapper_name= generatePackingStatements(s,syms,pdSyms, struct_decl );

  // Generate a call to the outlined function.
  SgScopeStatement * p_scope = s->get_scope();
  ROSE_ASSERT(p_scope);

  // Retest this...
  ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());

  SgStatement *func_call = NULL;
  if (use_dlopen) 
    // if dlopen() is used, insert a lib call to find the function pointer from a shared lib file
    // e.g. OUT__2__8072__p = findFunctionUsingDlopen("OUT__2__8072__", "OUT__2__8072__.so");
  {
    // build the return type of the lib call 
    SgFunctionParameterTypeList * tlist = buildFunctionParameterTypeList();
    (tlist->get_arguments()).push_back(buildPointerType(buildPointerType(buildVoidType())));

    SgFunctionType *ftype_return = buildFunctionType(buildVoidType(), tlist);
    // build the argument list
    string lib_name = output_path+"/"+func_name_str+".so"; 
    SgExprListExp* arg_list = buildExprListExp(buildStringVal(func_name_str), buildStringVal(lib_name)); 
    SgFunctionCallExp* dlopen_call = buildFunctionCallExp(SgName(FIND_FUNCP_DLOPEN),ftype_return,arg_list, p_scope);
    SgExprStatement * assign_stmt = buildAssignStatement(buildVarRefExp(func_name_str+"p",p_scope),dlopen_call);
    SageInterface::insertStatementBefore(s, assign_stmt);
    // Generate a function call using the func pointer
    // e.g. (*OUT__2__8888__p)(__out_argv2__1527__);
    SgExprListExp* exp_list_exp = SageBuilder::buildExprListExp();
    appendExpression(exp_list_exp, buildVarRefExp(wrapper_name,p_scope));
    func_call = buildFunctionCallStmt(buildPointerDerefExp(buildVarRefExp(func_name_str+"p",p_scope)), exp_list_exp);   
  }
  else  // regular function call for other cases
    func_call = generateCall (func, syms, readOnlyVars, wrapper_name,p_scope);

  ROSE_ASSERT (func_call != NULL);

  // Retest this...
  ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());

  // What is this doing (what happens to "s")
  //  cout<<"Debug before replacement(s, func_call), s is\n "<< s<<endl; 
  //     SageInterface::insertStatementAfter(s,func_call);
  SageInterface::replaceStatement(s,func_call);

  ROSE_ASSERT(s != NULL);
  ROSE_ASSERT(s->get_statements().empty() == true);

  // Retest this...
  ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());

  // Restore preprocessing information.
  ASTtools::moveInsidePreprocInfo (s, func->get_definition ()->get_body ());
  ASTtools::pastePreprocInfoFront (ppi_before, func_call);
  ASTtools::pastePreprocInfoBack  (ppi_after, func_call);

  SageInterface::fixVariableReferences(p_scope);

  //-----------handle dependent declarations, headers if new file is generated-------------
  if (new_file)
  {
    SageInterface::fixVariableReferences(new_file);
    // SgProject * project2= new_file->get_project();
    // AstTests::runAllTests(project2);// turn it off for now
    // project2->unparse();
  }

  // Retest this...
  ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());
#if 0
  printf ("After resetting the parent: func->get_definition() = %p func->get_definition()->get_body()->get_parent() = %p \n",func->get_definition(),func->get_definition()->get_body()->get_parent());
#endif

  ROSE_ASSERT(s != NULL);
  ROSE_ASSERT(s->get_statements().empty() == true);

  if (useNewFile == true)
  {
    // DQ (2/6/2009): I need to write this function to support the
    // insertion of the function into the specified scope.  If the
    // file associated with the scope is marked as compiler generated 
    // (or as a transformation) then the declarations referenced in the 
    // function must be copied as well (those not in include files)
    // and the include files must be copies also. If the SgFile
    // is not compiler generated (or a transformation) then we just
    // append the function to the scope (trivial case).

    // I am passing in the target_func so that I can get the location 
    // in the file from which we want to generate a matching context.
    // It would be better if this were the location of the new function call
    // to the outlined function (since dependent declaration in the function
    // containing the outlined code (loop nest, for example) might contain
    // relevant typedefs which have to be created in the new file (or the 
    // outlined function as a special case).

#if 1
    ROSE_ASSERT(func->get_firstNondefiningDeclaration() != NULL);
    ROSE_ASSERT(TransformationSupport::getSourceFile(func) == TransformationSupport::getSourceFile(func->get_firstNondefiningDeclaration()));
    ROSE_ASSERT(TransformationSupport::getSourceFile(func->get_scope()) == TransformationSupport::getSourceFile(func->get_firstNondefiningDeclaration()));
#if 0
    printf ("******************************************************************** \n");
    printf ("Now calling SageInterface::appendStatementWithDependentDeclaration() \n");
    printf ("******************************************************************** \n");
#endif
    // If the outline function will be placed into it's own file then we need to reconstruct any dependent statements (and #include CPP directives).
    SgFunctionDeclaration* func_orig = const_cast<SgFunctionDeclaration *> (SageInterface::getEnclosingFunctionDeclaration (s));
    SageInterface::appendStatementWithDependentDeclaration(func,glob_scope,func_orig,exclude_headers);
    // printf ("DONE: Now calling SageInterface::appendStatementWithDependentDeclaration() \n");
#else
    printf ("Skipping call to SageInterface::appendStatementWithDependentDeclaration() (testing only)\n");
#endif
  }

#if 1
  // DQ (2/26/2009): Moved (here) to as late as possible so that all transformations are complete before running AstPostProcessing()

  // This fails for moreTest3.cpp
  // Run the AST fixup on the AST for the source file.
  SgSourceFile* originalSourceFile = TransformationSupport::getSourceFile(src_scope);
  //     printf ("##### Calling AstPostProcessing() on SgFile = %s \n",originalSourceFile->getFileName().c_str());
  AstPostProcessing (originalSourceFile);
  //     printf ("##### DONE: Calling AstPostProcessing() on SgFile = %s \n",originalSourceFile->getFileName().c_str());
#else
  printf ("Skipping call to AstPostProcessing (originalSourceFile); \n");
#endif

  ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());

  if (useNewFile == true)
  {
#if 1
    // This fails for moreTest3.cpp
    // Run the AST fixup on the AST for the separate file of outlined code.
    SgSourceFile* separateOutlinedSourceFile = TransformationSupport::getSourceFile(glob_scope);
    //          printf ("##### Calling AstPostProcessing() on SgFile = %s \n",separateOutlinedSourceFile->getFileName().c_str());
    AstPostProcessing (separateOutlinedSourceFile);
    //          printf ("##### DONE: Calling AstPostProcessing() on SgFile = %s \n",separateOutlinedSourceFile->getFileName().c_str());
#else
    printf ("Skipping call to AstPostProcessing (separateOutlinedSourceFile); \n");
#endif
  }

  return Result (func, func_call, new_file);
}

/* For a set of variables to be passed into the outlined function, 
 * generate the following statements before the call of the outlined function
 * used when useParameterWrapper is set to true
   	 void * __out_argv[2];
 	*(__out_argv +0)=(void*)(&var1);// better form: __out_argv[0]=(void*)(&var1);
  	*(__out_argv +1)=(void*)(&var2); //__out_argv[1]=(void*)(&var2);
 * return the name for the array parameter used to wrap all pointer parameters
 *
 * if Outliner::useStructureWrapper is true, we wrap parameters into a structure instead of an array.
 * In this case, we need to know the structure type's name and parameters passed by pointers
 *  struct OUT__1__8228___data __out_argv1__1527__; 
 *  __out_argv1__1527__.i = i;
 *  __out_argv1__1527__.j = j;
 *  __out_argv1__1527__.sum_p = &sum; 
 *
 */
std::string Outliner::generatePackingStatements(SgStatement* target, ASTtools::VarSymSet_t & syms, ASTtools::VarSymSet_t & pdsyms, SgClassDeclaration* struct_decl /* = NULL */)
{

  int var_count = syms.size();
  int counter=0;
  string wrapper_name= generateFuncArgName(target); //"__out_argv";

  if (var_count==0) 
    return wrapper_name;
  SgScopeStatement* cur_scope = target->get_scope();
  ROSE_ASSERT( cur_scope != NULL);

  // void * __out_argv[count];
  SgType* my_type = NULL; 

  if (useStructureWrapper)
  {
    ROSE_ASSERT (struct_decl != NULL);
    my_type = struct_decl->get_type();
  }
  else // default case for parameter wrapping is to use an array of pointers
  { 
    SgType* pointer_type = buildPointerType(buildVoidType()); 
    my_type = buildArrayType(pointer_type, buildIntVal(var_count));
  }

  SgVariableDeclaration* out_argv = buildVariableDeclaration(wrapper_name, my_type, NULL,cur_scope);

  // Since we have moved the outlined block to be the outlined function's body, and removed it 
  // from its location in the original location where it was outlined, we can't insert new 
  // statements relative to "target".
  SageInterface::insertStatementBefore(target, out_argv);

  SgVariableSymbol * wrapper_symbol = getFirstVarSym(out_argv);
  ROSE_ASSERT(wrapper_symbol->get_parent() != NULL);
  //  cout<<"Inserting wrapper declaration ...."<<wrapper_symbol->get_name().getString()<<endl;
  for (ASTtools::VarSymSet_t::reverse_iterator i = syms.rbegin ();
      i != syms.rend (); ++i)
  {
    SgExpression * lhs = NULL;
    SgExpression * rhs = NULL;
    if (useStructureWrapper)
    {
      // if use a struct to wrap parameters
      // two kinds of field: original type v.s. pointer type to the original type
      //  __out_argv1__1527__.i = i;
      //  __out_argv1__1527__.sum_p = &sum;
      SgInitializedName* i_name = (*i)->get_declaration();
      SgVariableSymbol * i_symbol = const_cast<SgVariableSymbol *>(*i);
      //SgType* i_type = i_symbol->get_type();
       string member_name= i_name->get_name ().str ();
//     cout<<"Debug: Outliner::generatePackingStatements() symbol to be packed:"<<member_name<<endl;  
      rhs = buildVarRefExp(i_symbol); 
      if (pdsyms.find(i_symbol) != pdsyms.end()) // pointer type
      {
        member_name = member_name+"_p";
        // member_type = buildPointerType(member_type);
        //rhs = buildAddressOfOp(rhs); 
        rhs = buildCastExp( buildAddressOfOp(rhs), buildPointerType(buildVoidType())); 
      }
      SgClassDefinition* class_def = isSgClassDefinition (isSgClassDeclaration(struct_decl->get_definingDeclaration())->get_definition()) ; 
      ROSE_ASSERT (class_def != NULL);
      lhs = buildDotExp ( buildVarRefExp(out_argv), buildVarRefExp (member_name, class_def));  
    }
    else
    // Default case: array of pointers, e.g.,  *(__out_argv +0)=(void*)(&var1);
    {
      lhs = buildPntrArrRefExp(buildVarRefExp(wrapper_symbol),buildIntVal(counter));
      SgVarRefExp* rhsvar = buildVarRefExp((*i)->get_declaration(),cur_scope);
      rhs = buildCastExp( buildAddressOfOp(rhsvar), buildPointerType(buildVoidType()), SgCastExp::e_C_style_cast);
    }

    // build wrapping statement for either cases
    SgExprStatement * expstmti= buildAssignStatement(lhs,rhs);
    SageInterface::insertStatementBefore(target, expstmti);
    counter ++;
  }
  return wrapper_name; 
}

SgSourceFile* 
Outliner::generateNewSourceFile(SgBasicBlock* s, const string& file_name)
{
  SgSourceFile* new_file = NULL;
  SgProject * project = getEnclosingNode<SgProject> (s);
  ROSE_ASSERT(project != NULL);
  // s could be transformation generated, so use the root SgFile for file name
  SgFile* cur_file = getEnclosingNode<SgFile> (s);
  ROSE_ASSERT (cur_file != NULL);
  //grab the file suffix, 
  // TODO another way is to generate suffix according to source language type
  std::string orig_file_name = cur_file->get_file_info()->get_filenameString();
  //cout<<"debug:orig_file_name="<<orig_file_name<<endl;
  std::string file_suffix = StringUtility::fileNameSuffix(orig_file_name);
  ROSE_ASSERT(file_suffix !="");
  std::string new_file_name = file_name+"."+file_suffix;
  if (!output_path.empty())
  { // save the outlined function into a specified path
    new_file_name = StringUtility::stripPathFromFileName(new_file_name);
    new_file_name= output_path+"/"+new_file_name;
  }
  // remove pre-existing file with the same name
  remove (new_file_name.c_str());
  new_file = isSgSourceFile(buildFile(new_file_name, new_file_name,project));
  //new_file = isSgSourceFile(buildFile(new_file_name, new_file_name));
  ROSE_ASSERT(new_file != NULL);
  return new_file;
}

// eof
