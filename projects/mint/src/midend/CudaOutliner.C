/*
  Didem Unat
  
 */

#include <rose.h>
#include "CudaOutliner.h"       

#include "unparser.h"           //parses types into strings  

#include "mintTools/MintTools.h"
#include "Copy.hh"


#include <iostream>
#include <string>
#include <sstream>
#include <set>


//! Stores a variable symbol remapping.
typedef std::map<const SgVariableSymbol *, SgVariableSymbol *> VarSymRemap_t;


SgFunctionDeclaration *CudaOutliner::createFuncSkeleton (const string& name, SgType* ret_type,
							 SgFunctionParameterList* params, SgScopeStatement* scope)
{
  ROSE_ASSERT(scope != NULL);
  ROSE_ASSERT(isSgGlobal(scope)!=NULL);
  SgFunctionDeclaration* func;
  SgProcedureHeaderStatement* fortranRoutine;
  if (SageInterface::is_Fortran_language())
    {
      fortranRoutine = SageBuilder::buildProcedureHeaderStatement(name.c_str(),ret_type, params, SgProcedureHeaderStatement::e_subroutine_subprogram_kind,scope);
      func = isSgFunctionDeclaration(fortranRoutine);
    }
  else
    {
      func = SageBuilder::buildDefiningFunctionDeclaration(name,ret_type,params,scope);
    }

  ROSE_ASSERT (func != NULL);
  SgFunctionSymbol* func_symbol = scope->lookup_function_symbol(func->get_name());
  ROSE_ASSERT(func_symbol != NULL);
  if (0)//Outliner::enable_debug)
    {
      printf("Found function symbol in %p for function:%s\n",scope,func->get_name().getString().c_str());
    }
  return func;
}


void CudaOutliner::setKernelParams(int dimension, SgStatement* func_call_stmt, SgScopeStatement* scope)
{
  /*
  dim3 threads(chunksizeX, chunksizeY);

  int numBlocksX = n/ chunksizeX;
  int numBlocksY = n/ (chunksizeY*numRows);

  dim3 grid(numBlocksX, numBlocksY);
  */

  SgInitializedName* arg1 = buildInitializedName(SgName("BLOCKDIM_X"), buildIntType());
  SgInitializedName* arg2 = buildInitializedName(SgName("BLOCKDIM_Y"), buildIntType());

  SgType* dim3_t = buildOpaqueType("dim3", scope);

  SgExprListExp* exprList = buildExprListExp();
  //appendArg(exprList, arg1);
  //ppendArg(exprList, arg2);

  SgFunctionParameterList * paraList = buildFunctionParameterList();
  appendArg(paraList, arg1);
  appendArg(paraList, arg2);

  SgMemberFunctionDeclaration * funcdecl = buildNondefiningMemberFunctionDeclaration
  ("threads", dim3_t, paraList);

  SgConstructorInitializer* constr = 
    new SgConstructorInitializer(funcdecl, exprList, dim3_t, false, false,
				 false, false );

  string dimBlock = MintTools::generateBlockDimName(func_call_stmt);

  SgVariableDeclaration * dim_blocks_var_decl = buildVariableDeclaration(dimBlock, dim3_t, constr);

  /*
  string dimGrid = generateGridDimName(func_call_stmt);

  SgVariableDeclaration * dim_grid_var_decl = buildVariableDeclaration(dimGrid, dim3_t);				
  */

  cout<< dim_blocks_var_decl->unparseToString ()<< endl ;
}

SgStatement* CudaOutliner::generateCall (SgFunctionDeclaration* out_func,
					 MintHostSymToDevInitMap_t hostToDevVars,
					 const ASTtools::VarSymSet_t& syms,
					 SgScopeStatement* scope,
					 string dim_blocks_str,
					 string dim_grid_str)
{
  //Generates a call on the host to launch a cuda kernel 
  //1.step: check first the cuda kernel exists 
  //2.step: create argument list 
  //3.step: build a function expression 

  //cout << "INFO-mint GenerateCall Param :" << ASTtools::toString(syms) << endl;

  SgGlobal* glob_scope = TransformationSupport::getGlobalScope(scope);
  ROSE_ASSERT(glob_scope != NULL);

  SgFunctionSymbol* func_symbol = glob_scope->lookup_function_symbol(out_func->get_name());

  if (func_symbol == NULL)
    {
      printf("Failed to find a function symbol in %p for function %s\n", 
	     glob_scope, out_func->get_name().getString().c_str());
      ROSE_ASSERT(func_symbol != NULL);
    }

  ROSE_ASSERT (func_symbol);

  // Create an argument list.                                                                                                                           
  SgExprListExp* exp_list_exp = SageBuilder::buildExprListExp();
  ROSE_ASSERT (exp_list_exp);
 
  for (ASTtools::VarSymSet_t::const_iterator i = syms.begin ();  i != syms.end (); ++i)
    {
      SgVariableSymbol* host_sym = const_cast<SgVariableSymbol*> (*i);
      SgVarRefExp* v_ref = NULL;
      //we found a variable in the host to dev variable mapping 
      if(hostToDevVars.find(host_sym) != hostToDevVars.end())
	{
	  SgInitializedName* dev_name = hostToDevVars[host_sym];
	  ROSE_ASSERT(dev_name);
	  v_ref = SageBuilder::buildVarRefExp (dev_name);
	}
      else {
	//Scalar values 
	// Create variable reference to pass to the function.     
	v_ref = SageBuilder::buildVarRefExp (host_sym);
      }
	ROSE_ASSERT (v_ref);
	//cout << "Ref : " << isSgVarRefExp(v_ref)->get_symbol()->get_name().str() << endl ;     
	exp_list_exp->append_expression (v_ref);    
    } 

  // Generate the actual call.       
  // cuda function call has a 2nd parameter list <<<grid, therad, semsize, stream>>
  SgFunctionCallExp* func_call_expr = SageBuilder::buildFunctionCallExp(func_symbol, exp_list_exp);
  ROSE_ASSERT (func_call_expr);
  
  SgExprStatement *func_call_stmt = SageBuilder::buildExprStatement (func_call_expr);
  
  ROSE_ASSERT (func_call_stmt);

  SgType* dim3_type = buildOpaqueType("dim3", scope);
  SgVariableDeclaration* dim_grid_var_decl = buildVariableDeclaration(dim_grid_str, dim3_type);
  SgVariableDeclaration* dim_blocks_var_decl = buildVariableDeclaration(dim_blocks_str, dim3_type);

  SgVariableSymbol * dim_grid_var_sym = new SgVariableSymbol(*(dim_grid_var_decl->get_variables().begin()));

  //SageInterface::appendStatement (dim_blocks_var_decl, block);
  SgVariableSymbol * dim_blocks_var_sym = new SgVariableSymbol(*(dim_blocks_var_decl->get_variables().begin()));

 
  Sg_File_Info* sourceLocation = Sg_File_Info::generateDefaultFileInfoForTransformationNode();

  ROSE_ASSERT(sourceLocation);

  SgFunctionRefExp* func_ref =  buildFunctionRefExp(func_symbol);
  
  
  SgCudaKernelExecConfig* kernel_config =  
    new SgCudaKernelExecConfig(sourceLocation,
			       new SgVarRefExp(sourceLocation, dim_grid_var_sym),
			       new SgVarRefExp(sourceLocation, dim_blocks_var_sym),
			       NULL,
			       NULL
			       );
  ROSE_ASSERT(kernel_config);

  SgCudaKernelCallExp * cuda_call_site = 
    new SgCudaKernelCallExp(sourceLocation, func_ref, exp_list_exp, kernel_config); 
			    

  SgExprStatement *kernel_call_stmt = SageBuilder::buildExprStatement (cuda_call_site);

  return kernel_call_stmt;
}


//! Creates an SgInitializedName.
static
SgInitializedName *
createInitName (const string& name, SgType* type,
                SgDeclarationStatement* decl,
                SgScopeStatement* scope,
                SgInitializer* init = 0)
{
  SgName sg_name (name.c_str ());

// DQ (2/24/2009): Added assertion.
  ROSE_ASSERT(name.empty() == false);
  SgInitializedName* new_name = buildInitializedName (sg_name, type);
  ROSE_ASSERT (new_name);
  // Insert symbol
  if (scope)
    {
      SgVariableSymbol* new_sym = new SgVariableSymbol (new_name);
      scope->insert_symbol (sg_name, new_sym);
    }

  return new_name;
}

//! Stores a new outlined-function parameter.
typedef std::pair<string, SgType *> OutlinedFuncParam_t;

#if 0
//! Returns 'true' if the base type is a primitive type.
static
bool
isBaseTypePrimitive (const SgType* type)
{
  if (!type) return false;
  const SgType* base_type = type->findBaseType ();
  if (base_type)
    switch (base_type->variantT ())
      {
      case V_SgTypeBool:
      case V_SgTypeChar:
      case V_SgTypeDouble:
      case V_SgTypeFloat:
      case V_SgTypeInt:
      case V_SgTypeLong:
      case V_SgTypeLongDouble:
      case V_SgTypeLongLong:
      case V_SgTypeShort:
      case V_SgTypeSignedChar:
      case V_SgTypeSignedInt:
      case V_SgTypeSignedLong:
      case V_SgTypeSignedShort:
      case V_SgTypeUnsignedChar:
      case V_SgTypeUnsignedInt:
      case V_SgTypeUnsignedLong:
      case V_SgTypeUnsignedShort:
      case V_SgTypeVoid:
      case V_SgTypeWchar:
        return true;
      default:
        break;
      }
  return false;
}
#endif
/*!
 *  \brief Creates a new outlined-function parameter for a given
 *  variable. The requirement is to preserve data read/write semantics.
 *
 *  For C/C++: we use pointer dereferencing to implement pass-by-reference
 *    In a recent implementation, side effect analysis is used to find out
 *    variables which are not modified so pointer types are not used.
 *

 *
 *  Given a variable (i.e., its type and name) whose references are to
 *  be outlined, create a suitable outlined-function parameter. 
 *  For C/C++, the  parameter is created as a pointer, to support parameter passing of
 *  aggregate types in C programs. 
 *  Moreover, the type is made 'void' if the base type is not a primitive type.
 *   
 *  An original type may need adjustments before we can make a pointer type from it.
 *  For example: 
 *    a)Array types from a function parameter: its first dimension is auto converted to a pointer type 
 *
 *    b) Pointer to a C++ reference type is illegal, we create a pointer to its
 *    base type in this case. It also match the semantics for addressof(refType) 
 *
 * 
 *  The implementation follows two steps:
 *     step 1: adjust a variable's base type
 *     step 2: decide on its function parameter type
 *  Liao, 8/14/2009
 */
static OutlinedFuncParam_t createParam (const SgInitializedName* i_name, bool readOnly=false)
{

  ROSE_ASSERT (i_name);
  SgType* init_type = i_name->get_type();
  ROSE_ASSERT (init_type);

  // Stores the real parameter type to be used in new_param_type
  string init_name = i_name->get_name ().str (); 

  SgType* param_base_type;
  //has to be not scalar                                                                                                                                              
  if(isSgArrayType (init_type) ){

    SgType* base_type = init_type->findBaseType();
    param_base_type = buildPointerType(base_type);
  }
  else if (isSgPointerType (init_type))
    {
      //Didem, this used to be 2 but I changed into 0. Didn't make any sense
      init_name = init_name.substr(0);
      //Didem: I have changed the types into 1D pointer type 
      SgType* base_type = init_type->findBaseType();
      param_base_type = buildPointerType(base_type);

      //param_base_type = init_type;
    }
  else{
    param_base_type = init_type;
  }

   // The parameter name reflects the type: the same name means the same type, 
   // p__ means a pointer type
  string new_param_name = init_name;
  SgType* new_param_type = NULL;

  // For classic behavior, read only variables are passed by values for C/C++
  // They share the same name and type
  if (Outliner::enable_classic) 
  { 
    // read only parameter: pass-by-value, the same type and name
    // shared parameters  : already pointer type 
    //if (readOnly)
    {

      new_param_type = param_base_type;
    }
    /*
    else
    {
      new_param_name= "d__" + new_param_name;
      new_param_type = SgPointerType::createType (param_base_type);

      }*/
  }
  else // very conservative one, assume the worst side effects (all are written) 
  {
    new_param_name= new_param_name;
  }

  // So use base type directly
  // C/C++ parameters will use their new param type to implement pass-by-reference

  return OutlinedFuncParam_t (new_param_name, new_param_type);
}

//! Returns 'true' if the given type is 'const'.
static
bool
isReadOnlyType (const SgType* type)
{
  ROSE_ASSERT (type);

  const SgModifierType* mod = 0;
  switch (type->variantT ())
    {
    case V_SgModifierType:
      mod = isSgModifierType (type);
      break;
    case V_SgReferenceType:
      mod = isSgModifierType (isSgReferenceType (type)->get_base_type ());
      break;
    case V_SgPointerType:
      mod = isSgModifierType (isSgPointerType (type)->get_base_type ());
      break;
    default:
      mod = 0;
      break;
    }
  return mod
    && mod->get_typeModifier ().get_constVolatileModifier ().isConst ();
}

/*!
 *  \brief Creates an assignment to "pack" a local variable back into
 *  an outlined-function parameter that has been passed as a pointer
 *  value.
 *
 *  This routine takes the original "unpack" definition, of the form
 *
 *    TYPE local_unpack_var = *outlined_func_arg;
 *    int i = *(int *)(__out_argv[1]); // parameter wrapping case
 *
 *  and creates the "re-pack" assignment expression,
 *
 *    *outlined_func_arg = local_unpack_var
 *    *(int *)(__out_argv[1]) =i; // parameter wrapping case
 *
 *  C++ variables of reference types do not need this step.
 */
static
SgAssignOp *
createPackExpr (SgInitializedName* local_unpack_def)
{
  if (!Outliner::temp_variable)
  {
    if (is_C_language()) //skip for pointer dereferencing used in C language
      return NULL;
  }
  // reference types do not need copy the value back in any cases
  if (isSgReferenceType (local_unpack_def->get_type ()))  
    return NULL;

  if (local_unpack_def
      && !isReadOnlyType (local_unpack_def->get_type ()))
//      && !isSgReferenceType (local_unpack_def->get_type ()))
    {
      SgName local_var_name (local_unpack_def->get_name ());

      SgAssignInitializer* local_var_init =
        isSgAssignInitializer (local_unpack_def->get_initializer ());
      ROSE_ASSERT (local_var_init);

      // Create the LHS, which derefs the function argument, by
      // copying the original dereference expression.
      // 
      SgPointerDerefExp* param_deref_unpack =
        isSgPointerDerefExp (local_var_init->get_operand_i ());
      if (param_deref_unpack == NULL)  
      {
        cout<<"packing statement is:"<<local_unpack_def->get_declaration()->unparseToString()<<endl;
        cout<<"local unpacking stmt's initializer's operand has non-pointer deferencing type:"<<local_var_init->get_operand_i ()->class_name()<<endl;
        ROSE_ASSERT (param_deref_unpack);
      }

      SgPointerDerefExp* param_deref_pack = isSgPointerDerefExp (ASTtools::deepCopy (param_deref_unpack));
      ROSE_ASSERT (param_deref_pack);
              
      // Create the RHS, which references the local variable.
      SgScopeStatement* scope = local_unpack_def->get_scope ();
      ROSE_ASSERT (scope);
      SgVariableSymbol* local_var_sym =
        scope->lookup_var_symbol (local_var_name);
      ROSE_ASSERT (local_var_sym);
      SgVarRefExp* local_var_ref = SageBuilder::buildVarRefExp (local_var_sym);
      ROSE_ASSERT (local_var_ref);

      // Assemble the final assignment expression.
      return SageBuilder::buildAssignOp (param_deref_pack, local_var_ref);
    }
  return 0;
}

/*!
 *  \brief Creates a pack statement 
 *
 *  This routine creates an SgExprStatement wrapper around the return
 *  of createPackExpr.
 *  
 *  void OUT__1__4305__(int *ip__,int *sump__)
 * {
 *   int i =  *((int *)ip__);
 *   int sum =  *((int *)sump__);
 *   for (i = 0; i < 100; i++) {
 *     sum += i;
 *   }
 *  //The following are called (re)pack statements
 *    *((int *)sump__) = sum;
 *    *((int *)ip__) = i;
}

 */
static
SgExprStatement *
createPackStmt (SgInitializedName* local_unpack_def)
{
  // No repacking for Fortran for now
  if (local_unpack_def==NULL || SageInterface::is_Fortran_language())
    return NULL;
  SgAssignOp* pack_expr = createPackExpr (local_unpack_def);
  if (pack_expr)
    return SageBuilder::buildExprStatement (pack_expr);
  else
    return 0;
}


/*!
 *  \brief Records a mapping between two variable symbols, and record
 *  the new symbol.
 *
 *  This routine creates the target variable symbol from the specified
 *  SgInitializedName object. If the optional scope is specified
 *  (i.e., is non-NULL), then this routine also inserts the new
 *  variable symbol into the scope's symbol table.
 */

static void
recordSymRemap (const SgVariableSymbol* orig_sym,
                SgInitializedName* name_new,
                SgScopeStatement* scope,
                VarSymRemap_t& sym_remap)
{
  if (orig_sym && name_new)
    { //TODO use the existing symbol associated with name_new!
      // DQ (2/24/2009): Added assertion.
      ROSE_ASSERT(name_new->get_name().is_null() == false);

      SgVariableSymbol* sym_new = new SgVariableSymbol (name_new);
      ROSE_ASSERT (sym_new);
      sym_remap.insert (VarSymRemap_t::value_type (orig_sym, sym_new));

      if (scope)
        {
          scope->insert_symbol (name_new->get_name (), sym_new);
          name_new->set_scope (scope);
        }
    }
}

/*!
 *  \brief Records a mapping between variable symbols.
 *
 *  \pre The variable declaration must contain only 1 initialized
 *  name.
 */
static
void
recordSymRemap (const SgVariableSymbol* orig_sym,
                SgVariableDeclaration* new_decl,
                SgScopeStatement* scope,
                VarSymRemap_t& sym_remap)
{
  if (orig_sym && new_decl)
    {
      SgInitializedNamePtrList& vars = new_decl->get_variables ();
      ROSE_ASSERT (vars.size () == 1);
      for (SgInitializedNamePtrList::iterator i = vars.begin ();
           i != vars.end (); ++i)
        recordSymRemap (orig_sym, *i, scope, sym_remap);
    }
}
// handle OpenMP private variables
// pSyms: private variable set
// scope: the scope of a private variable's local declaration
// private_remap: a map between the original variables and their private copies
static void handlePrivateVariables( const ASTtools::VarSymSet_t& pSyms,
                                    SgScopeStatement* scope, 
                                    VarSymRemap_t& private_remap)
{
  // --------------------------------------------------
  for (ASTtools::VarSymSet_t::const_reverse_iterator i = pSyms.rbegin ();
      i != pSyms.rend (); ++i)
  {
    const SgInitializedName* i_name = (*i)->get_declaration ();
    ROSE_ASSERT (i_name);
    string name_str = i_name->get_name ().str ();
    SgType * v_type = i_name->get_type();
    SgVariableDeclaration* local_var_decl = buildVariableDeclaration(name_str, v_type, NULL, scope);
    prependStatement (local_var_decl,scope);
    recordSymRemap (*i, local_var_decl, scope, private_remap);
  }
}
// Create one parameter for an outlined function
// return the created parameter
SgInitializedName* createOneFunctionParameter(const SgInitializedName* i_name, 
					      bool readOnly, 
					      SgFunctionDeclaration* func)
			
{
  ROSE_ASSERT (i_name);
  ROSE_ASSERT (func);
  SgFunctionParameterList* params = func->get_parameterList ();
  ROSE_ASSERT (params);
  SgFunctionDefinition* def = func->get_definition ();
  ROSE_ASSERT (def);

  // It handles language-specific details internally, like pass-by-value, pass-by-reference
  // name and type is not enough, need the SgInitializedName also for tell 
  // if an array comes from a parameter list
  OutlinedFuncParam_t param = createParam (i_name,readOnly);
 
  SgName p_sg_name (param.first.c_str ());
  // name, type, declaration, scope, 
  // TODO function definition's declaration should not be passed to createInitName()
  SgInitializedName* p_init_name = createInitName (param.first, param.second, def->get_declaration(), def);

  ROSE_ASSERT (p_init_name);
  prependArg(params,p_init_name);
  return p_init_name;
}

// ===========================================================
//! Fixes up references in a block to point to alternative symbols.
// based on an existing symbol-to-symbol map
// Also called variable substitution. 
static void
remapVarSyms (const VarSymRemap_t& vsym_remap,  // regular shared variables
              const ASTtools::VarSymSet_t& pdSyms, // special shared variables
              const VarSymRemap_t& private_remap,  // variables using private copies
              SgBasicBlock* b)
{
  // Check if variable remapping is even needed.
  if (vsym_remap.empty() && private_remap.empty()){
    //cout << "no remapping " << endl ;
    return;
  }
  // Find all variable references
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t refs = NodeQuery::querySubTree (b, V_SgVarRefExp);
  // For each of the references , 
  for (NodeList_t::iterator i = refs.begin (); i != refs.end (); ++i)
  {
    // Reference possibly in need of fix-up.
    SgVarRefExp* ref_orig = isSgVarRefExp (*i);
    ROSE_ASSERT (ref_orig);

    // Search for a symbol which needs to be replaced.
    // cout << "this is the symbol " <<ref_orig->get_symbol()->get_name().str() << endl ;

    VarSymRemap_t::const_iterator ref_new =  vsym_remap.find (ref_orig->get_symbol ());
    VarSymRemap_t::const_iterator ref_private =  private_remap.find (ref_orig->get_symbol ());

    // a variable could be both a variable needing passing original value and private variable 
    // such as OpenMP firstprivate, lastprivate and reduction variable
    // For variable substitution, private remap has higher priority 
    // remapping private variables
    if (ref_private != private_remap.end()) 
    {
      // get the replacement variable
      SgVariableSymbol* sym_new = ref_private->second;
      // Do the replacement
      ref_orig->set_symbol (sym_new);
    }
    else if (ref_new != vsym_remap.end ()) // Needs replacement, regular shared variables
    {
      if(Outliner::enable_debug)
	{
	  cout << "replace this " << "variable with that " << endl ;
	}
      SgVariableSymbol* sym_new = ref_new->second;
      if (Outliner::temp_variable || Outliner::useStructureWrapper)
        // uniform handling if temp variables of the same type are used
      {// two cases: variable using temp vs. variables using pointer dereferencing!

        if (pdSyms.find(ref_orig->get_symbol())==pdSyms.end()) //using temp
          ref_orig->set_symbol(sym_new);
        else
        {
          SgPointerDerefExp * deref_exp = SageBuilder::buildPointerDerefExp(buildVarRefExp(sym_new));
          deref_exp->set_need_paren(true);
          SageInterface::replaceExpression(isSgExpression(ref_orig),isSgExpression(deref_exp));

        }
      }
      else // no variable cloning is used
      {
        if (is_C_language()) 
          // old method of using pointer dereferencing indiscriminately for C input
          // TODO compare the orig and new type, use pointer dereferencing only when necessary
        {
          SgPointerDerefExp * deref_exp = SageBuilder::buildPointerDerefExp(buildVarRefExp(sym_new));
          deref_exp->set_need_paren(true);
          SageInterface::replaceExpression(isSgExpression(ref_orig),isSgExpression(deref_exp));
        }
        else
          ref_orig->set_symbol (sym_new);
      }
    } //find an entry
  } // for every refs

}


/*!
 *  \brief Creates new function parameters for a set of variable symbols.
 *
 *  We have several options for the organization of function parameters:
 *
 *  1. default: each variable to be passed has a function parameter
 *           To support C programs, this routine assumes parameters passed
 *           using pointers (rather than references).  
 *  2, useParameterWrapper: use an array as the function parameter, each
 *              pointer stores the address of the variable to be passed
 *  3. useStructureWrapper: use a structure, each field stores a variable's
 *              value or address according to use-by-address or not semantics
 *
 *  Moreover, it inserts "unpacking/unwrapping" and "repacking" statements at the 
 *  beginning and end of the function body, respectively, when necessary
 *
 *  Finally this routine records the mapping between the given variable symbols and the new
 *  symbols corresponding to the new parameters. This is used later on for variable replacement
 *
 */

void CudaOutliner::functionParameterHandling(ASTtools::VarSymSet_t& syms, // regular (shared) parameters
					     MintHostSymToDevInitMap_t hostToDevVars,		                      
					     const ASTtools::VarSymSet_t& pdSyms, // those must use pointer dereference
					     const ASTtools::VarSymSet_t& pSyms, 
					     // private variables, handles dead variables (neither livein nor liveout) 
					     std::set<SgInitializedName*> & readOnlyVars,
					     std::set<SgInitializedName*> & liveOutVars,
					     SgFunctionDeclaration* func) // the outlined function
{

  //ASTtools::VarSymSet_t syms;
  //std::copy(syms1.begin(), syms1.end(), std::inserter(syms,syms.begin()));

  VarSymRemap_t sym_remap;     // variable remapping for regular(shared) variables
  VarSymRemap_t private_remap; // variable remapping for private/firstprivate/reduction variables
  ROSE_ASSERT (func);
  SgFunctionParameterList* params = func->get_parameterList ();
  ROSE_ASSERT (params);
  SgFunctionDefinition* def = func->get_definition ();
  ROSE_ASSERT (def);
  SgBasicBlock* body = def->get_body ();
  ROSE_ASSERT (body);

  // Place in which to put new outlined variable symbols.
  SgScopeStatement* args_scope = isSgScopeStatement (body);
  ROSE_ASSERT (args_scope);

  // For each variable symbol, create an equivalent function parameter. 
  // Also create unpacking and repacking statements.
  int counter=0;
//  SgInitializedName* parameter1=NULL; // the wrapper parameter
  SgVariableDeclaration*  local_var_decl  =  NULL;

  // handle OpenMP private variables/ or those which are neither live-in or live-out
  handlePrivateVariables(pSyms, body, private_remap);

  // --------------------------------------------------
  // for each parameters passed to the outlined function
  // They include parameters for regular shared variables and 
  // also the shared copies for firstprivate and reduction variables

  for (ASTtools::VarSymSet_t::reverse_iterator i = syms.rbegin ();i != syms.rend (); ++i)
  {
    // Basic information about the variable to be passed into the outlined function
    // Variable symbol name
    const SgInitializedName* i_name = (*i)->get_declaration ();
    ROSE_ASSERT (i_name);
    string name_str = i_name->get_name ().str ();

    SgName p_sg_name ( name_str);
    //SgType* i_type = i_name->get_type ();
    bool readOnly = false;
    if (readOnlyVars.find(const_cast<SgInitializedName*> (i_name)) != readOnlyVars.end())
      readOnly = true;

    // step 1. Create parameters and insert it into the parameter list.
    // ----------------------------------------
    SgInitializedName* p_init_name = NULL;
    SgVariableSymbol* host_sym= const_cast<SgVariableSymbol*> (*i);
    
    if(hostToDevVars.find(host_sym) != hostToDevVars.end() ){
      //these are vector variables
      SgInitializedName* dev_name = hostToDevVars[host_sym];
      p_init_name = createInitName (dev_name->get_name(), dev_name->get_type(), func, def);

      ROSE_ASSERT (p_init_name);
      prependArg(func->get_parameterList (),p_init_name);
    }
    else{
      //scalar values
      p_init_name = createOneFunctionParameter(i_name, readOnly, func); 
    }
    // step 2. Create unpacking/unwrapping statements, also record variables to be replaced
    // ----------------------------------------
   // bool isPointerDeref = false; 

    if (Outliner::enable_classic) 
      {  // classic methods use parameters directly, no unpacking is needed
	if (!readOnly) 
	  //read only variable should not have local variable declaration, using parameter directly
	  // taking advantage of the same parameter names for readOnly variables
	  // Let postprocessing to patch up symbols for them
	  {
	    // non-readonly variables need to be mapped to their parameters with different names (p__)
	    // remapVarSyms() will use pointer dereferencing for all of them by default in C, 
	    // this is enough to mimic the classic outlining work 
	    //handleSharedVariables(*i, p_init_name, args_scope, sym_remap);
	    //handleSharedVariables(*i, body, sym_remap);

	    //Didem: I comment out this part because it uses pointer deferencing for all non-readonly variables.
	    //recordSymRemap(*i,p_init_name, args_scope, sym_remap); 
	  }
      }
    else 
      { 
	local_var_decl  = NULL; //createUnpackDecl (p_init_name, counter, isPointerDeref, i_name , NULL, body);
	ROSE_ASSERT (local_var_decl);
	prependStatement (local_var_decl,body);
	// regular and shared variables used the first local declaration
        recordSymRemap (*i, local_var_decl, args_scope, sym_remap);
	// transfer the value for firstprivate variables. 
      }


    // step 3. Create and insert companion re-pack statement in the end of the function body
    // If necessary
    // ----------------------------------------
    SgInitializedName* local_var_init = NULL;
    if (local_var_decl != NULL )
      local_var_init = local_var_decl->get_decl_item (SgName (name_str.c_str ()));

    if (!SageInterface::is_Fortran_language() && !Outliner::enable_classic)  
      ROSE_ASSERT(local_var_init!=NULL);  


    SgExprStatement* pack_stmt = createPackStmt (local_var_init);
    if (pack_stmt)
      appendStatement (pack_stmt,body);
    
    counter ++;
  } //end for

  // variable substitution
  SgBasicBlock* func_body = func->get_definition()->get_body();
  remapVarSyms (sym_remap, pdSyms, private_remap , func_body);
}

SgFunctionDeclaration * CudaOutliner::generateFunction ( SgBasicBlock* s,
                                                         const string& func_name_str,
                                                         ASTtools::VarSymSet_t& syms,
							 MintHostSymToDevInitMap_t hostToDevVars,
							 const ASTtools::VarSymSet_t& pdSyms,
                                                         const ASTtools::VarSymSet_t& psyms,
                                                         SgScopeStatement* scope)
{
  //Create a function named 'func_name_str', with a parameter list from 'syms'                                                             
  //pdSyms specifies symbols which must use pointer dereferencing if replaced during outlining,  
  //only used when -rose:outline:temp_variable is used                                                                                     
  //psyms are the symbols for OpenMP private variables, or dead variables (not live-in, not live-out)    

  ROSE_ASSERT ( s && scope);
  ROSE_ASSERT(isSgGlobal(scope));

  // step 1: perform necessary liveness and side effect analysis, if requested.     
  // ---------------------------------------------------------                                                                           
  std::set< SgInitializedName *> liveIns, liveOuts;
  // Collect read-only variables of the outlining target                                                                                
  std::set<SgInitializedName*> readOnlyVars;
  if (Outliner::temp_variable||Outliner::enable_classic)
    {
      SgStatement* firstStmt = (s->get_statements())[0];
      if (isSgForStatement(firstStmt)&& Outliner::enable_liveness)
        {
          LivenessAnalysis * liv = SageInterface::call_liveness_analysis (SageInterface::getProject());
	  SageInterface::getLiveVariables(liv, isSgForStatement(firstStmt), liveIns, liveOuts);
        }
      SageInterface::collectReadOnlyVariables(s,readOnlyVars);
      if (0)//Outliner::enable_debug)
        {
          cout<<"  INFO:Mint: CudaOutliner::generateFunction()---Found "<<readOnlyVars.size()<<" read only variables..:";
          for (std::set<SgInitializedName*>::const_iterator iter = readOnlyVars.begin();
               iter!=readOnlyVars.end(); iter++)
            cout<<" "<<(*iter)->get_name().getString()<<" ";
          cout<<endl;
          cout<<"CudaOutliner::generateFunction() -----Found "<<liveOuts.size()<<" live out variables..:";
          for (std::set<SgInitializedName*>::const_iterator iter = liveOuts.begin();
               iter!=liveOuts.end(); iter++)
            cout<<" "<<(*iter)->get_name().getString()<<" ";
          cout<<endl;
        }
    }
    //step 2. Create function skeleton, 'func'.             
    // -----------------------------------------                
  SgName func_name (func_name_str);
  SgFunctionParameterList *parameterList = buildFunctionParameterList();
  
  SgType* func_Type = SgTypeVoid::createType ();
  SgFunctionDeclaration* func = createFuncSkeleton (func_name, func_Type ,parameterList, scope);

  //adds __global__ keyword 
  func->get_functionModifier().setCudaKernel();
 
  ROSE_ASSERT (func);

  // Liao, 4/15/2009 , enforce C-bindings  for C++ outlined code 
  // enable C code to call this outlined function                                                                                
  // Only apply to C++ , pure C has trouble in recognizing extern "C"                                                    
  // Another way is to attach the function with preprocessing info:                                                     
  // #if __cplusplus                                                                                                           
  // extern "C"                                                                                                              
  // #endif                                                                                                                                
  // We don't choose it since the language linkage information is not explicit in AST                                                           
  if ( SageInterface::is_Cxx_language() || is_mixed_C_and_Cxx_language() \
       || is_mixed_Fortran_and_Cxx_language() || is_mixed_Fortran_and_C_and_Cxx_language() )
    {
      // Make function 'extern "C"'                                                                                                                                         
      func->get_declarationModifier().get_storageModifier().setExtern();
      func->set_linkage ("C");
    }

  //step 3. Create the function body                                                                                       
  // -----------------------------------------                                                                                              
  // Generate the function body by deep-copying 's'.                                                                                       
              
  SgBasicBlock* func_body = func->get_definition()->get_body();
  ROSE_ASSERT (func_body != NULL);

  // This does a copy of the statements in "s" to the function body of the outlined function.                                             
  ROSE_ASSERT(func_body->get_statements().empty() == true);

  // This calls AST copy on each statement in the SgBasicBlock, but not on the block, so the                                          
  // symbol table is setup by AST copy mechanism and it is  setup properly
  SageInterface::moveStatementsBetweenBlocks (s, func_body);

  if (Outliner::useNewFile)
    ASTtools::setSourcePositionAtRootAndAllChildrenAsTransformation(func_body);

  //step 4: variable handling, including:                                                                                                  
  // -----------------------------------------                                                                                             
  //   create parameters of the outlined functions                                                                                        
  //   add statements to unwrap the parameters if wrapping is requested
  //   add repacking statements if necessary                                                                                               
  //   replace variables to access to parameters, directly or indirectly                                                                  
  //   do not wrap parameters 
  Outliner::enable_classic = true;

  functionParameterHandling(syms, hostToDevVars, pdSyms, psyms, readOnlyVars, liveOuts, func);
  ROSE_ASSERT (func != NULL);
  
  // Retest this...                                                                                                       
  func->set_type(buildFunctionType(func->get_type()->get_return_type(), buildFunctionParameterTypeList(func->get_parameterList())));
  ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());

  ROSE_ASSERT(scope->lookup_function_symbol(func->get_name()));

  return func;
}  

// eof


