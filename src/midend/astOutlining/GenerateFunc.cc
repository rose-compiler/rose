/**
 *  \file Transform/GenerateFunc.cc
 *
 *  \brief Generates an outlined (independent) C-callable function
 *  from an SgBasicBlock.
 *
 *  This outlining implementation specifically generates C-callable
 *  routines for use in an empirical tuning application. Such routines
 *  can be isolated into their own, dynamically shareable modules.
 */
#include <rose.h>
#include <iostream>
#include <string>
#include <sstream>
#include <set>


#include "Transform.hh"
#include "ASTtools.hh"
#include "VarSym.hh"
#include "Copy.hh"
#include "StmtRewrite.hh"
#include "Outliner.hh"

//! Stores a variable symbol remapping.
typedef std::map<const SgVariableSymbol *, SgVariableSymbol *> VarSymRemap_t;

// =====================================================================

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

/* ===========================================================
 */

//! Creates a non-member function.
static
SgFunctionDeclaration *
createFuncSkeleton (const string& name, SgType* ret_type,
                    SgFunctionParameterList* params, SgScopeStatement* scope)
   {
     ROSE_ASSERT(scope != NULL);
     ROSE_ASSERT(isSgGlobal(scope)!=NULL);
#if 0     
     SgFunctionType *func_type = new SgFunctionType (ret_type, false);
     ROSE_ASSERT (func_type);
#endif
     SgFunctionDeclaration* func;
     SgProcedureHeaderStatement* fortranRoutine;
  // Liao 12/13/2007, generate SgProcedureHeaderStatement for Fortran code
     if (SageInterface::is_Fortran_language()) 
        {
#if 0
          fortranRoutine = new SgProcedureHeaderStatement(ASTtools::newFileInfo (), name, func_type);
          fortranRoutine->set_subprogram_kind(SgProcedureHeaderStatement::e_subroutine_subprogram_kind);

          func = isSgFunctionDeclaration(fortranRoutine);   
#endif
          fortranRoutine = SageBuilder::buildProcedureHeaderStatement(name.c_str(),ret_type, params, SgProcedureHeaderStatement::e_subroutine_subprogram_kind,scope);
          func = isSgFunctionDeclaration(fortranRoutine);  
        }
       else
        {
       // DQ (2/24/2009): Modified to use SageBuild functions with Liao.
       // func = new SgFunctionDeclaration (ASTtools::newFileInfo (), name, func_type);
          func = SageBuilder::buildDefiningFunctionDeclaration(name,ret_type,params,scope);
        }

     ROSE_ASSERT (func != NULL);

#if 0
  // This code is placed in a conditional because the Fortran case (above) does not use the SagBuilder 
  // functions.  Once it does then this should not be required.  I will debug the Fortran case with 
  // Liao after we get the C/C++ case finished.
     if (SageInterface::is_Fortran_language()) 
        {
       // DQ (9/7/2007): Fixup the defining and non-defining declarations
          ROSE_ASSERT(func->get_definingDeclaration() == NULL);
          func->set_definingDeclaration(func);
          ROSE_ASSERT(func->get_firstNondefiningDeclaration() != func);

          SgFunctionDefinition *func_def = new SgFunctionDefinition (ASTtools::newFileInfo (), func);
          func_def->set_parent (func);  //necessary or not?

          SgBasicBlock *func_body = new SgBasicBlock (ASTtools::newFileInfo ());
          func_def->set_body (func_body);
          func_body->set_parent (func_def);

          func->set_parameterList (params);
          params->set_parent (func);
        }
#endif        

   SgFunctionSymbol* func_symbol = scope->lookup_function_symbol(func->get_name());
   ROSE_ASSERT(func_symbol != NULL);
   if (Outliner::enable_debug)
   {
     printf("Found function symbol in %p for function:%s\n",scope,func->get_name().getString().c_str());
   }
     return func;
   }

// ===========================================================

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
  SgInitializedName* new_name = new SgInitializedName (ASTtools::newFileInfo (), sg_name, type, init,decl, scope, 0);
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

/*!
 *  \brief Creates a new outlined-function parameter for a given
 *  variable.
 *
 *  Given a variable (i.e., its type and name) whose references are to
 *  be outlined, create a suitable outlined-function parameter. The
 *  parameter is created as a pointer, to support parameter passing of
 *  aggregate types in C programs. Moreover, the type is made 'void'
 *  if the base type is not a primitive type.
 */
static
OutlinedFuncParam_t
createParam (const SgInitializedName* i_name, bool readOnly=false)
//createParam (const string& init_name, const SgType* init_type, bool readOnly=false)
{
  ROSE_ASSERT (i_name);
  SgType* init_type = i_name->get_type();
  ROSE_ASSERT (init_type);
  SgType* param_base_type = 0;
  if (isBaseTypePrimitive (init_type)||Outliner::enable_classic)
    // for classic translation, there is no additional unpacking statement to 
    // convert void* type to non-primitive type of the parameter
    // So we don't convert the type to void* here
  {
    // Duplicate the initial type.
    param_base_type = init_type; //!< \todo Is shallow copy here OK?
    //param_base_type = const_cast<SgType *> (init_type); //!< \todo Is shallow copy here OK?
    // convert the first dimension of an array type function parameter to a pointer type, Liao
    // 4/24/2009
    if (isSgArrayType(param_base_type)) 
      if (isSgFunctionDefinition(i_name->get_scope()))
        param_base_type= SageBuilder::buildPointerType(isSgArrayType(param_base_type)->get_base_type());
     
    ROSE_ASSERT (param_base_type);
  }
  else
  {
    param_base_type = SgTypeVoid::createType ();
    ROSE_ASSERT (param_base_type);
    //Take advantage of the const modifier
    if (ASTtools::isConstObj (init_type))
    {
      SgModifierType* mod = new SgModifierType (param_base_type);
      ROSE_ASSERT (mod);
      mod->get_typeModifier ().get_constVolatileModifier ().setConst ();
      param_base_type = mod;
    }
  }

  // Stores the new parameter.
   string init_name = i_name->get_name ().str (); 
  string new_param_name = init_name;
  SgType* new_param_type = NULL;
  // For classic behavior, read only variables are passed by values for C/C++
  // They share the same name and type
  if (Outliner::enable_classic) 
  { 
    // read only parameter: pass-by-value, the same type and name
    if (readOnly)
    {
      new_param_type = param_base_type;
    }
    else
    {
      new_param_name+= "p__";
      new_param_type = SgPointerType::createType (param_base_type);

    }
  }
  else
  {
    new_param_name+= "p__";
    if (!SageInterface::is_Fortran_language())
    {
      new_param_type = SgPointerType::createType (param_base_type);
      ROSE_ASSERT (new_param_type);
    }
  }

  if (SageInterface::is_Fortran_language())
    return OutlinedFuncParam_t (new_param_name,param_base_type);
  else 
    return OutlinedFuncParam_t (new_param_name, new_param_type);
}

/*!
 *  \brief Creates a local variable declaration to "unpack" an
 *  outlined-function's parameter that has been passed as a pointer
 *  value.
 *  int index is optionally used as an offset inside a wrapper parameter for multiple variables
 *
 *  OUT_XXX(int *ip__)
 *  {
 *    // This is call unpacking declaration, Liao, 9/11/2008
 *   int i = * (int *) ip__;
 *  }
 *
 *  Or
 *
 *  OUT_XXX (void * __out_argv[n])
 *  {
 *    int * _p_i =  (int*)__out_argv[0];
 *    int * _p_j =  (int*)__out_argv[1];
 *    ....
 *  }
 * The key is to set local_name, local_type, and local_val for all cases
 */
static
SgVariableDeclaration *
createUnpackDecl (SgInitializedName* param, int index,
                  bool isPointerDeref, 
                  const SgInitializedName* i_name, // original variable to be passed as parameter
                 // const string& local_var_name,
                 // SgType* local_var_type, 
                  SgScopeStatement* scope)
{
  ROSE_ASSERT(param&&scope && i_name);
  const string local_var_name = i_name->get_name().str();
  SgType* local_var_type = i_name ->get_type();

  // Convert an array type parameter's first dimension to a pointer type
  // Liao, 4/24/2009
  if (isSgArrayType(local_var_type)) 
    if (isSgFunctionDefinition(i_name->get_scope()))
      local_var_type = SageBuilder::buildPointerType(isSgArrayType(local_var_type)->get_base_type());

  // Create an expression that "unpacks" (dereferences) the parameter.
  // SgVarRefExp* 
  SgExpression* param_ref = buildVarRefExp(param,scope);
  if (Outliner::useParameterWrapper) // using index for a wrapper parameter
  {
     param_ref= buildPntrArrRefExp(param_ref,buildIntVal(index));
  } 

  // the original data type of the variable
  SgType* param_deref_type = const_cast<SgType *> (local_var_type);
  ROSE_ASSERT (param_deref_type);

  // Cast from 'void *' to 'LOCAL_VAR_TYPE *'
  SgReferenceType* ref = isSgReferenceType (param_deref_type);
  SgType* local_var_type_ptr =
    SgPointerType::createType (ref ? ref->get_base_type (): param_deref_type);
  ROSE_ASSERT (local_var_type_ptr);
  SgCastExp* cast_expr = buildCastExp(param_ref,local_var_type_ptr,SgCastExp::e_C_style_cast);

  // the right hand of the declaration
  SgPointerDerefExp* param_deref_expr = NULL;
  param_deref_expr = buildPointerDerefExp(cast_expr);

  // Declare a local variable to store the dereferenced argument.
  SgName local_name (local_var_name.c_str ());

  if (SageInterface::is_Fortran_language())
    local_name = SgName(param->get_name());

  // The value of the assignment statement

  // DQ (2/24/2009): Modified construction of variable initializer.
  // SgAssignInitializer* local_val = buildAssignInitializer(param_deref_expr);
     SgAssignInitializer* local_val = NULL;
     if (SageInterface::is_Fortran_language())
        {
          local_val = NULL;
        }
       else
        {
          if (Outliner::temp_variable)
             {
            // int* ip = (int *)(__out_argv[1]); // isPointerDeref == true
            // int i = *(int *)(__out_argv[1]);
               if (isPointerDeref)
                    local_val = buildAssignInitializer(cast_expr); // casting is enough for pointer types
                 else // temp variable need additional dereferencing from the parameter on the right side
                    local_val = buildAssignInitializer(buildPointerDerefExp(cast_expr));
             } 
            else
             {
               if  (is_C_language()) // using pointer dereferences
                  {
                    local_val = buildAssignInitializer(cast_expr);
                  }
                 else
                  {
                    if  (is_Cxx_language()) // using pointer dereferences
                       {
                         SgPointerDerefExp* param_deref_expr = buildPointerDerefExp(cast_expr);
                      // printf ("In createUnpackDecl(): param_deref_expr = %p \n",param_deref_expr);

                         local_val = buildAssignInitializer(param_deref_expr);
                       }
                      else
                       {
                         printf ("No other languages are supported by outlining currently. \n");
                         ROSE_ASSERT(false);
                       }
                  }
             }
        }
  
// printf ("In createUnpackDecl(): local_val = %p \n",local_val);

  SgType* local_type = NULL;
  // Rich's idea was to leverage C++'s reference type: two cases:
  //  a) for variables of reference type: no additional work
  //  b) for others: make a reference type to them
  //   all variable accesses in the outlined function will have
  //   access the address of the by default, not variable substitution is needed 
  // but this method won't work for non-C++ code, where & on left side of assignment 
  //  is not recognized at all.
  if (SageInterface::is_Fortran_language())
    local_type= local_var_type;
  else if (Outliner::temp_variable) 
  // unique processing for C/C++ if temp variables are used
  {
    if (isPointerDeref)
      local_type = buildPointerType(param_deref_type);
    else
      local_type = param_deref_type;
  }  
  else  
  {
    if (is_C_language())
    {   
      //have to use pointer dereference
      local_type = buildPointerType(param_deref_type);
    }
    else // C++ language
    { 
      local_type= isSgReferenceType(param_deref_type)
        ?param_deref_type:SgReferenceType::createType(param_deref_type);
    }
  }
  ROSE_ASSERT (local_type);

  SgVariableDeclaration* decl = buildVariableDeclaration(local_name,local_type,local_val,scope);

// printf ("In createUnpackDecl(): getFirstInitializedName(decl)->get_initializer() = %p \n",SageInterface::getFirstInitializedName(decl)->get_initializer());

  return decl;
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
 *
 */
static
SgAssignOp *
createPackExpr (SgInitializedName* local_unpack_def)
{
  if (!Outliner::temp_variable)
  {
    if (is_C_language()) //skip for pointer dereferencing used in C language
      return NULL;
    if (isSgReferenceType (local_unpack_def->get_type ()))  
      return NULL;
  }
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
      SgVarRefExp* local_var_ref = new SgVarRefExp (ASTtools::newFileInfo (),
                                                    local_var_sym);
      ROSE_ASSERT (local_var_ref);

      // Assemble the final assignment expression.
      return new SgAssignOp (ASTtools::newFileInfo (),
                             param_deref_pack, local_var_ref);
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
    return new SgExprStatement (ASTtools::newFileInfo (), pack_expr);
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
static
void
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

/*!
 *  \brief Creates new function parameters for a set of variable
 *  symbols.
 *
 *  In addition to creating the function parameters, this routine
 *  records the mapping between the given variable symbols and the new
 *  symbols corresponding to the new parameters. 
 *  This is used later on for variable replacement
 *
 *  To support C programs, this routine assumes parameters passed
 *  using pointers (rather than references).  
 *
 *  Moreover, it inserts "packing" and "unpacking" statements at the 
 *  beginning and end of the function declaration, respectively, when necessary
 */
static
void
appendParams (const ASTtools::VarSymSet_t& syms,
              const ASTtools::VarSymSet_t& pdSyms,
              std::set<SgInitializedName*> & readOnlyVars,
              std::set<SgInitializedName*> & liveOutVars,
              SgFunctionDeclaration* func,
              VarSymRemap_t& sym_remap)
{
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
  SgInitializedName* parameter1=NULL; // the wrapper parameter
  for (ASTtools::VarSymSet_t::const_reverse_iterator i = syms.rbegin ();
      i != syms.rend (); ++i)
  {
    // Basic information about the variable to be passed into the outlined function
    // Variable symbol name
    const SgInitializedName* i_name = (*i)->get_declaration ();
    ROSE_ASSERT (i_name);
    string name_str = i_name->get_name ().str ();
    SgName p_sg_name (name_str);
    //SgType* i_type = i_name->get_type ();
    bool readOnly = false;
    if (readOnlyVars.find(const_cast<SgInitializedName*> (i_name)) != readOnlyVars.end())
      readOnly = true;
    // Create parameters and insert it into the parameter list.
    // ----------------------------------------
    SgInitializedName* p_init_name = NULL;
    // Case 1: using a wrapper for all variables 
    // all wrapped parameters have pointer type
    if (Outliner::useParameterWrapper)
    {
      if (i==syms.rbegin())
      {
        SgName var1_name = "__out_argv";
        SgType* ptype= buildPointerType(buildPointerType(buildVoidType()));
        parameter1 = buildInitializedName(var1_name,ptype);
        appendArg(params,parameter1);
      }
      p_init_name = parameter1; // set the source parameter to the wrapper
    }
    else // case 2: use a parameter for each variable
    {
      // It handles language-specific details internally, like pass-by-value, pass-by-reference
      // name and type is not enough, need the SgInitializedName also for tell 
      // if an array comes from a parameter list
      OutlinedFuncParam_t param = createParam (i_name,readOnly);
      //OutlinedFuncParam_t param = createParam (name_str, i_type,readOnly);
      SgName p_sg_name (param.first.c_str ());
      // name, type, declaration, scope, 
      // TODO function definition's declaration should not be passed to createInitName()
      p_init_name = createInitName (param.first, param.second, 
                                   def->get_declaration(), def);
      ROSE_ASSERT (p_init_name);
      prependArg(params,p_init_name);
    }

    // Create unpacking statements:transfer values or addresses from parameters
    // ----------------------------------------
    bool isPointerDeref = false; 
    SgVariableDeclaration*  local_var_decl  =  NULL;
    if (Outliner::temp_variable) 
    { // Check if the current variable belongs to the symbol set 
      //suitable for using pointer dereferencing
      const SgVariableSymbol* i_sym = isSgVariableSymbol(i_name->get_symbol_from_symbol_table ());
      ROSE_ASSERT(i_sym!=NULL);
      if ( pdSyms.find(i_sym)!=pdSyms.end())
        isPointerDeref = true;
    }  

    if (Outliner::enable_classic)
    {
      if (readOnly) 
      {
        //read only variable should not have local variable declaration, using parameter directly
        // taking advantage of the same parameter names for readOnly variables
        // Let postprocessing to patch up symbols for them

      }
      else
      {
        // non-readonly variables need to be mapped to their parameters with different names (p__)
        // remapVarSyms() will use pointer dereferencing for all of them by default in C, 
        // this is enough to mimic the classic outlining work 
        recordSymRemap(*i,p_init_name, args_scope, sym_remap); 
      }
    } else 
    {
      local_var_decl  = 
             createUnpackDecl (p_init_name, counter, isPointerDeref, i_name ,body);
             //createUnpackDecl (p_init_name, counter, isPointerDeref, name_str, i_type,body);
      ROSE_ASSERT (local_var_decl);
      prependStatement (local_var_decl,body);
      if (SageInterface::is_Fortran_language())
        args_scope = NULL; // not sure about Fortran scope
      recordSymRemap (*i, local_var_decl, args_scope, sym_remap);
    }

    // Create and insert companion re-pack statement in the end of the function body
    // If necessary
    // ----------------------------------------
    SgInitializedName* local_var_init = NULL;
    if (local_var_decl != NULL )
     local_var_init = local_var_decl->get_decl_item (SgName (name_str.c_str ()));

    if (!SageInterface::is_Fortran_language() && !Outliner::enable_classic)  
      ROSE_ASSERT(local_var_init!=NULL);  

    // Only generate restoring statement for non-pointer dereferencing cases
    // if temp variable mode is enabled
    if (Outliner::temp_variable)
    {
      if(!isPointerDeref)
      {
        //conservatively consider them as all live out if no liveness analysis is enabled,
        bool isLiveOut = true;
       if (Outliner::enable_liveness)
         if (liveOutVars.find(const_cast<SgInitializedName*> (i_name))==liveOutVars.end())
            isLiveOut = false;

        // generate restoring statements for written and liveOut variables:
        //  isWritten && isLiveOut --> !isRead && isLiveOut --> (findRead==NULL && findLiveOut!=NULL)
        // must compare to the original init name (i_name), not the local copy (local_var_init)
        if (readOnlyVars.find(const_cast<SgInitializedName*> (i_name))==readOnlyVars.end() && isLiveOut)   // variables not in read-only set have to be restored
       {
          if (Outliner::enable_debug)
            cout<<"Generating restoring statement for non-read-only variable:"<<local_var_init->unparseToString()<<endl;

          SgExprStatement* pack_stmt = createPackStmt (local_var_init);
          if (pack_stmt)
            appendStatement (pack_stmt,body);
        }
        else
        {
          if (Outliner::enable_debug)
            cout<<"skipping a read-only variable for restoring its value:"<<local_var_init->unparseToString()<<endl;
        }
      } else
      {
        if (Outliner::enable_debug)
          cout<<"skipping a variable using pointer-dereferencing for restoring its value:"<<local_var_init->unparseToString()<<endl;
      }
    }
    else
    {
      SgExprStatement* pack_stmt = createPackStmt (local_var_init);
      if (pack_stmt)
        appendStatement (pack_stmt,body);
    }
    counter ++;
  } //end for
}

// ===========================================================

//! Fixes up references in a block to point to alternative symbols.
// based on an existing symbol-to-symbol map
static
void
remapVarSyms (const VarSymRemap_t& vsym_remap, const ASTtools::VarSymSet_t& pdSyms, SgBasicBlock* b)
{
  if (!vsym_remap.empty ()) // Check if any remapping is even needed.
  {
    typedef Rose_STL_Container<SgNode *> NodeList_t;
    NodeList_t refs = NodeQuery::querySubTree (b, V_SgVarRefExp);
    for (NodeList_t::iterator i = refs.begin (); i != refs.end (); ++i)
    {
      // Reference possibly in need of fix-up.
      SgVarRefExp* ref_orig = isSgVarRefExp (*i);
      ROSE_ASSERT (ref_orig);

      // Search for a replacement symbol.
      VarSymRemap_t::const_iterator ref_new =
        vsym_remap.find (ref_orig->get_symbol ());
      if (ref_new != vsym_remap.end ()) // Needs replacement
      {
        SgVariableSymbol* sym_new = ref_new->second;
        if (Outliner::temp_variable)
        // uniform handling if temp variables of the same type are used
        {// two cases: variable using temp vs. variables using pointer dereferencing!

          if (pdSyms.find(ref_orig->get_symbol())==pdSyms.end()) //using temp
            ref_orig->set_symbol (sym_new);
          else
          {
            SgPointerDerefExp * deref_exp = SageBuilder::buildPointerDerefExp(buildVarRefExp(sym_new));
            deref_exp->set_need_paren(true);
            SageInterface::replaceExpression(isSgExpression(ref_orig),isSgExpression(deref_exp));

          }
        }
        else
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
}


// =====================================================================

// DQ (2/25/2009): Modified function interface to pass "SgBasicBlock*" as not const parameter.
//! Create a function named 'func_name_str', with a parameter list from 'syms'
// pdSyms specifies symbols which must use pointer dereferencing if replaced during outlining, only used when -rose:outline:temp_variable is used
SgFunctionDeclaration *
Outliner::Transform::generateFunction ( SgBasicBlock* s,
                                          const string& func_name_str,
                                          const ASTtools::VarSymSet_t& syms,
                                          const ASTtools::VarSymSet_t& pdSyms,
                                          SgScopeStatement* scope)
{
  ROSE_ASSERT (s&&scope);
  ROSE_ASSERT(isSgGlobal(scope));
  std::set< SgInitializedName *> liveIns, liveOuts;
   // Collect read only variables of the outlining target
    std::set<SgInitializedName*> readOnlyVars;
    if (Outliner::temp_variable||Outliner::enable_classic)
    {
      SgStatement* firstStmt = (s->get_statements())[0];
      if (isSgForStatement(firstStmt)&& enable_liveness)
      {
        LivenessAnalysis * liv = SageInterface::call_liveness_analysis (SageInterface::getProject());
        SageInterface::getLiveVariables(liv, isSgForStatement(firstStmt), liveIns, liveOuts);
      }
      SageInterface::collectReadOnlyVariables(s,readOnlyVars);
      if (Outliner::enable_debug)
      {
        cout<<"Outliner::Transform::generateFunction() -----Found "<<readOnlyVars.size()<<" read only variables..:";
        for (std::set<SgInitializedName*>::const_iterator iter = readOnlyVars.begin();
            iter!=readOnlyVars.end(); iter++)
          cout<<" "<<(*iter)->get_name().getString()<<" ";
        cout<<endl;
        cout<<"Outliner::Transform::generateFunction() -----Found "<<liveOuts.size()<<" live out variables..:";
        for (std::set<SgInitializedName*>::const_iterator iter = liveOuts.begin();
            iter!=liveOuts.end(); iter++)
          cout<<" "<<(*iter)->get_name().getString()<<" ";
        cout<<endl;
      }
    }

  // Create function skeleton, 'func'.
     SgName func_name (func_name_str);
     SgFunctionParameterList *parameterList = buildFunctionParameterList();

     SgFunctionDeclaration* func = createFuncSkeleton (func_name,SgTypeVoid::createType (),parameterList, scope);
     ROSE_ASSERT (func);

#if 0
  // DQ (2/24/2009): This is already built by the SageBuilder  functions called in createFuncSkeleton().
  // Liao 10/30/2007 maintain the symbol table
     SgFunctionSymbol * func_symbol = new SgFunctionSymbol(func);
     printf ("In Outliner::Transform::generateFunction(): func_symbol = %p \n",func_symbol);
     const_cast<SgBasicBlock *>(s)->insert_symbol(func->get_name(), func_symbol);
#endif

#if 1 // Liao, 4/15/2009 , enable C code to call this outlined function
  // Only apply to C++ , pure C has trouble in recognizing extern "C"
  // Another way is to attach the function with preprocessing info:
  // #if __cplusplus 
  // extern "C"
  // #endif
  // We don't choose it since the language linkage information is not explicit in AST
  // if (!SageInterface::is_Fortran_language())
     if ( SageInterface::is_Cxx_language() || is_mixed_C_and_Cxx_language() || is_mixed_Fortran_and_Cxx_language() || is_mixed_Fortran_and_C_and_Cxx_language() )
        {
       // Make function 'extern "C"'
          func->get_declarationModifier().get_storageModifier().setExtern();
          func->set_linkage ("C");
        }
#endif
  // Generate the function body by deep-copying 's'.
     SgBasicBlock* func_body = func->get_definition()->get_body();
     ROSE_ASSERT (func_body != NULL);

  // This does a copy of the statements in "s" to the function body of the outlined function.
     ROSE_ASSERT(func_body->get_statements().empty() == true);
#if 0
  // This calls AST copy on each statement in the SgBasicBlock, but not on the block, so the 
  // symbol table is not setup by AST copy mechanism and not setup properly by the outliner.
     ASTtools::appendStmtsCopy (s, func_body);
#else
     SageInterface::moveStatementsBetweenBlocks (s, func_body);
#endif

     if (Outliner::useNewFile)
          ASTtools::setSourcePositionAtRootAndAllChildrenAsTransformation(func_body);

#if 0
  // We can't call this here because "s" is passed in as "cont".
  // DQ (2/24/2009): I think that at this point we should delete the subtree represented by "s"
  // But it might have made more sense to not do a deep copy on "s" in the first place.
  // Why is there a deep copy on "s"?
     SageInterface::deleteAST(s);
#endif

#if 0
  // Liao, 12/27/2007, for DO .. CONTINUE loop, bug 171
  // copy a labeled CONTINUE at the end when it is missing
  // SgFortranDo --> SgLabelSymbol --> SgLabelStatement (CONTINUE)
  // end_numeric_label  fortran_statement    numeric_label
  if (SageInterface::is_Fortran_language())
  {
    SgStatementPtrList stmtList = func_body->get_statements();
    ROSE_ASSERT(stmtList.size()>0);
    SgStatementPtrList::reverse_iterator stmtIter;
    stmtIter = stmtList.rbegin();
    SgFortranDo * doStmt = isSgFortranDo(*stmtIter);
    if (doStmt) {
      SgLabelSymbol* label1= doStmt->get_end_numeric_label();
      if (label1)
      {
        SgLabelSymbol* label2=isSgLabelSymbol(ASTtools::deepCopy(label1));
        ROSE_ASSERT(label2);
        SgLabelStatement * contStmt = isSgLabelStatement(ASTtools::deepCopy(label1->\
              get_fortran_statement()));
        ROSE_ASSERT(contStmt);

        func_body->insert_symbol(label2->get_name(),isSgSymbol(label2));
        doStmt->set_end_numeric_label(label2);
        contStmt->set_numeric_label(label2);
        func_body->append_statement(contStmt);
      }
    } // end doStmt
  }
#endif

  // Store parameter list information.
     VarSymRemap_t vsym_remap;

  // Create parameters for outlined vars, and fix-up symbol refs in
  // the body.
  appendParams (syms, pdSyms,readOnlyVars, liveOuts,func, vsym_remap);
  remapVarSyms (vsym_remap, pdSyms, func_body);

     ROSE_ASSERT (func != NULL);

  // Retest this...
     ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());
  // printf ("After resetting the parent: func->get_definition() = %p func->get_definition()->get_body()->get_parent() = %p \n",func->get_definition(),func->get_definition()->get_body()->get_parent());
  //
     ROSE_ASSERT(scope->lookup_function_symbol(func->get_name()));
     return func;
   }

// eof
