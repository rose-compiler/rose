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
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include <iostream>
#include <string>
#include <sstream>
#include <set>


#include "Outliner.hh"
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
     SgFunctionDeclaration* func;
     SgProcedureHeaderStatement* fortranRoutine;
  // Liao 12/13/2007, generate SgProcedureHeaderStatement for Fortran code
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
  setOneSourcePositionForTransformation (new_name);
  ROSE_ASSERT (new_name);
  // Insert symbol
  if (scope)
    {
      SgVariableSymbol* new_sym = new SgVariableSymbol (new_name);
      scope->insert_symbol (sg_name, new_sym);
      ROSE_ASSERT (new_sym->get_parent() != NULL);
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
 *  variable. The requirement is to preserve data read/write semantics.
 *
 *  For C/C++: we use pointer dereferencing to implement pass-by-reference
 *    In a recent implementation, side effect analysis is used to find out
 *    variables which are not modified so pointer types are not used.
 *
 *  For Fortran, all parameters are passed by reference by default.
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
static
OutlinedFuncParam_t
createParam (const SgInitializedName* i_name, bool readOnly=false)
//createParam (const string& init_name, const SgType* init_type, bool readOnly=false)
{
  ROSE_ASSERT (i_name);
  SgType* init_type = i_name->get_type();
  ROSE_ASSERT (init_type);

  // Store the adjusted original types into param_base_type
  // primitive types: --> original type
  // complex types: void
  // array types from function parameters:  pointer type for 1st dimension
  // C++ reference type: use base type since we want to have uniform way to generate a pointer to the original type
  SgType* param_base_type = 0;
  if (isBaseTypePrimitive (init_type)||Outliner::enable_classic)
    // for classic translation, there is no additional unpacking statement to 
    // convert void* type to non-primitive type of the parameter
    // So we don't convert the type to void* here
  {
    // Duplicate the initial type.
    param_base_type = init_type; //!< \todo Is shallow copy here OK?
    //param_base_type = const_cast<SgType *> (init_type); //!< \todo Is shallow copy here OK?
   
    // Adjust the original types for array or function types (TODO function types) which are passed as function parameters 
    // convert the first dimension of an array type function parameter to a pointer type, 
    // This is called the auto type conversion for function or array typed variables 
    // that are passed as function parameters
    // Liao 4/24/2009
    if (isSgArrayType(param_base_type)) 
      if (isSgFunctionDefinition(i_name->get_scope()))
        param_base_type= SageBuilder::buildPointerType(isSgArrayType(param_base_type)->get_base_type());
     
    //For C++ reference type, we use its base type since pointer to a reference type is not allowed
    //Liao, 8/14/2009
    SgReferenceType* ref = isSgReferenceType (param_base_type);
    if (ref != NULL)
      param_base_type = ref->get_base_type();

    ROSE_ASSERT (param_base_type);
  }
  else // for non-primitive types, we use void as its base type
  {
    param_base_type = SgTypeVoid::createType ();
    ROSE_ASSERT (param_base_type);
    //Take advantage of the const modifier
    if (ASTtools::isConstObj (init_type))
    {
#if 0      // Liao, 8/13/2010, change to use new interfaces on top of the type table.
      SgModifierType* mod = SageBuilder::buildModifierType (param_base_type);
      ROSE_ASSERT (mod);
      mod->get_typeModifier ().get_constVolatileModifier ().setConst ();
#else
      SgModifierType* mod = SageBuilder::buildConstType(param_base_type);
#endif      
      param_base_type = mod;
    }
  }

  // Stores the real parameter type to be used in new_param_type
   string init_name = i_name->get_name ().str (); 
   // The parameter name reflects the type: the same name means the same type, 
   // p__ means a pointer type
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
  else // very conservative one, assume the worst side effects (all are written) 
  {
    if (!SageInterface::is_Fortran_language())
    {
      new_param_type = SgPointerType::createType (param_base_type);
      ROSE_ASSERT (new_param_type);
      new_param_name+= "p__";
    }
    else
      new_param_name= "s_"+new_param_name; //s_ means shared variables

  }

  // Fortran parameters are passed by reference by default,
  // So use base type directly
  // C/C++ parameters will use their new param type to implement pass-by-reference
  if (SageInterface::is_Fortran_language())
    return OutlinedFuncParam_t (new_param_name,param_base_type);
  else 
    return OutlinedFuncParam_t (new_param_name, new_param_type);
}

/*!
 *  \brief Creates a local variable declaration to "unpack" an
 *  outlined-function's parameter 
 *  int index is optionally used as an offset inside a wrapper parameter for multiple variables
 *
 *  The key is to set local_name, local_type, and local_val for all cases
 *
 *  There are three choices:
 *  Case 1: unpack one variable from one  parameter
 *  -----------------------------------------------
 *  OUT_XXX(int *ip__)
 *  {
 *    // This is called unpacking declaration for a read-only variable, Liao, 9/11/2008
 *   int i = * (int *) ip__;
 *  }
 *
 *  Case 2: unpack one variable from an array of pointers
 *  -----------------------------------------------
 *  OUT_XXX (void * __out_argv[n]) // for written variables, we have to use pointers
 *  {
 *    int * _p_i =  (int*)__out_argv[0];
 *    int * _p_j =  (int*)__out_argv[1];
 *    ....
 *  }
 *
 *  Case 3: unpack one variable from a structure
 *  -----------------------------------------------
 *  OUT__xxx (struct OUT__xxx__data *__out_argv)
 *  {
 *    int i = __out_argv->i;
 *    int j = __out_argv->j;
 *    int (*sum)[100UL] = __out_argv->sum_p;*
 *  }
 *
 * case 1 and case 2 have two variants: 
 *   using conventional pointer dereferencing or 
 *   using cloned variables(temp_variable)
 *
 */
static
SgVariableDeclaration *
createUnpackDecl (SgInitializedName* param, // the function parameter
                  int index, // the index to the array of pointers type
                  bool isPointerDeref, // must use pointer deference or not
                  const SgInitializedName* i_name, // original variable to be passed as the function parameter
                  SgClassDeclaration* struct_decl, // the struct declaration type used to wrap parameters 
                  SgScopeStatement* scope) // the scope into which the statement will be inserted 
{
  ROSE_ASSERT(param && scope && i_name);

  // keep the original name 
  const string orig_var_name = i_name->get_name().str();

 //---------------step 1 -----------------------------------------------
 // decide on the type : local_type
  // the original data type of the variable passed via parameter
  SgType* orig_var_type = i_name ->get_type();

  if (! SageInterface::is_Fortran_language())
  {  
    // Convert an array type parameter's first dimension to a pointer type
    // This conversion is implicit for C/C++ language. 
    // We have to make it explicit to get the right type
    // Liao, 4/24/2009  TODO we should only adjust this for the case 1
    if (isSgArrayType(orig_var_type)) 
      if (isSgFunctionDefinition(i_name->get_scope()))
        orig_var_type = SageBuilder::buildPointerType(isSgArrayType(orig_var_type)->get_base_type());
  }

  SgType* local_type = NULL;
  if (SageInterface::is_Fortran_language())
    local_type= orig_var_type;
  else if (Outliner::temp_variable || Outliner::useStructureWrapper) 
  // unique processing for C/C++ if temp variables are used
  {
    if (isPointerDeref) // use pointer dereferencing for some
    {
      local_type = buildPointerType(orig_var_type);
    }
    else // use variable clone instead for others
      local_type = orig_var_type;
  }  
  else // all other cases: non-fortran, not using variable clones 
  {
    if (is_C_language())
    {   
      // we use pointer types for all variables to be passed
      // the classic outlining will not use unpacking statement, but use the parameters directly.
      // So we can safely always use pointer dereferences here
      local_type = buildPointerType(orig_var_type);
    }
    else // C++ language
  // Rich's idea was to leverage C++'s reference type: two cases:
  //  a) for variables of reference type: no additional work
  //  b) for others: make a reference type to them
  //   all variable accesses in the outlined function will have
  //   access the address of the by default, not variable substitution is needed 
    { 
      local_type= isSgReferenceType(orig_var_type) ?orig_var_type: SgReferenceType::createType(orig_var_type);
    }
  }
  ROSE_ASSERT (local_type);

  SgAssignInitializer* local_val = NULL;

  // Declare a local variable to store the dereferenced argument.
  SgName local_name (orig_var_name.c_str ());
  if (SageInterface::is_Fortran_language())
    local_name = SgName(param->get_name());

  // This is the right hand of the assignment we want to build
  //
  // ----------step  2. Create the right hand expression------------------------------------
   // No need to have right hand for fortran
  if (SageInterface::is_Fortran_language())
     {
       local_val = NULL;
       return buildVariableDeclaration(local_name,local_type,local_val,scope);
     }
  // for non-fortran language
  // Create an expression that "unpacks" the parameter.
  //   case 1: default is to use the parameter directly
  //   case 2:  for array of pointer type parameter,  build an array element reference
  //   case 3: for structure type parameter, build a field reference
  //     we use type casting to have generic void * parameter and void* data structure fields
  //     so less types are to exposed during translation
  //   class Hello **this__ptr__ = (class Hello **)(((struct OUT__1__1527___data *)__out_argv) -> this__ptr___p);
  SgExpression* param_ref = NULL;
  if (Outliner::useParameterWrapper) // using index for a wrapper parameter
  {
    if (Outliner::useStructureWrapper)
    { // case 3: structure type parameter
      if (struct_decl != NULL)
      {
        SgClassDefinition* struct_def = isSgClassDeclaration(struct_decl->get_definingDeclaration())->get_definition();
        ROSE_ASSERT (struct_def != NULL);
        string field_name = orig_var_name;
        if (isPointerDeref)
          field_name = field_name+"_p";
        // __out_argv->i  or __out_argv->sum_p , depending on if pointer deference is needed
        //param_ref = buildArrowExp(buildVarRefExp(param, scope), buildVarRefExp(field_name, struct_def));
        //We use void* for all pointer types elements within the data structure. So type casting is needed here
  //e.g.   class Hello **this__ptr__ = (class Hello **)(((struct OUT__1__1527___data *)__out_argv) -> this__ptr___p);
        param_ref = buildCastExp(buildArrowExp(buildCastExp(buildVarRefExp(param, scope), buildPointerType(struct_decl->get_type())), 
                                               buildVarRefExp(field_name, struct_def)),
                                 local_type);
        //local_type
      }
      else
      {
        cerr<<"Outliner::createUnpackDecl(): no need to unpack anything since struct_decl is NULL."<<endl;
        ROSE_ASSERT (false);
      }
    }
    else // case 2: array of pointers 
    {
       param_ref= buildPntrArrRefExp(buildVarRefExp(param, scope),buildIntVal(index));
    }
  } 
  else // default case 1:  each variable has a pointer typed parameter , 
    // this is not necessary but we have a classic model for optimizing this
  {
     param_ref = buildVarRefExp(param, scope);
  }

   ROSE_ASSERT (param_ref != NULL);

  if (Outliner::useStructureWrapper)
  {
    // Or for structure type paramter
    // int (*sum)[100UL] = __out_argv->sum_p; // is PointerDeref type
    // int i = __out_argv->i;
    local_val = buildAssignInitializer(param_ref); 
  }
  else
  {
     //TODO: This is only needed for case 2 or C++ case 1, 
     // not for case 1 and case 3 since the source right hand already has the right type
     // since the array has generic void* elements. We need to cast from 'void *' to 'LOCAL_VAR_TYPE *'
     // Special handling for C++ reference type: addressOf (refType) == addressOf(baseType) 
     // So unpacking it to baseType* 
     SgReferenceType* ref = isSgReferenceType (orig_var_type);
     SgType* local_var_type_ptr =  SgPointerType::createType (ref ? ref->get_base_type (): orig_var_type);
     ROSE_ASSERT (local_var_type_ptr);
     SgCastExp* cast_expr = buildCastExp(param_ref,local_var_type_ptr,SgCastExp::e_C_style_cast);

    if (Outliner::temp_variable) // variable cloning is enabled
       {
      // int* ip = (int *)(__out_argv[1]); // isPointerDeref == true
      // int i = *(int *)(__out_argv[1]);
         if (isPointerDeref)
         {
              local_val = buildAssignInitializer(cast_expr); // casting is enough for pointer types
         }
           else // temp variable need additional dereferencing from the parameter on the right side
           {
              local_val = buildAssignInitializer(buildPointerDerefExp(cast_expr));
           }
       } 
    else // conventional pointer dereferencing algorithm
     {
       // int* ip = (int *)(__out_argv[1]);
       if  (is_C_language()) // using pointer dereferences
          {
            local_val = buildAssignInitializer(cast_expr);
          }
       else if  (is_Cxx_language()) 
         // We use reference type in the outlined function's body for C++ 
         // need the original value from a dereferenced type
         // using pointer dereferences to get the original type
         //  we use reference type instead of pointer type for C++
/*
 extern "C" void OUT__1__8452__(int *ip__,int *jp__,int (*sump__)[100UL])
{
  int &i =  *((int *)ip__);
  int &j =  *((int *)jp__);
  int (&sum)[100UL] =  *((int (*)[100UL])sump__);
  ...
} 
 */ 
          {
            local_val = buildAssignInitializer(buildPointerDerefExp(cast_expr));
           }
       else
          {
            printf ("No other languages are supported by outlining currently. \n");
            ROSE_ASSERT(false);
          }
     }
   }
// printf ("In createUnpackDecl(): local_val = %p \n",local_val);
//

  SgVariableDeclaration* decl = buildVariableDeclaration(local_name,local_type,local_val,scope);
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
  
  // Liao 10/26/2009, Most of time, using data structure of parameters don't need copy 
  // a local variable's value back to its original parameter
  if (Outliner::useStructureWrapper)
  {
    if (is_Cxx_language())
      return NULL;
  }

  // We expect that the value transferring back to the original parameter is only 
  // needed for variable clone options and when the variable is being written. 
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
  if (vsym_remap.empty() && private_remap.empty())
    return;
  // Find all variable references
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t refs = NodeQuery::querySubTree (b, V_SgVarRefExp);
  // For each of the references , 
  for (NodeList_t::iterator i = refs.begin (); i != refs.end (); ++i)
  {
    // Reference possibly in need of fix-up.
    SgVarRefExp* ref_orig = isSgVarRefExp (*i);
    ROSE_ASSERT (ref_orig);

    // Search for a symbol which need to be replaced.
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
static
void
variableHandling(const ASTtools::VarSymSet_t& syms, // regular (shared) parameters
              const ASTtools::VarSymSet_t& pdSyms, // those must use pointer dereference
              const ASTtools::VarSymSet_t& pSyms,  // private variables , this is kept to handle dead variables (neither livein nor liveout: TODO) 
              std::set<SgInitializedName*> & readOnlyVars,
              std::set<SgInitializedName*> & liveOutVars,
              SgClassDeclaration* struct_decl,
              SgFunctionDeclaration* func) // the outlined function
{
  VarSymRemap_t sym_remap; // variable remapping for regular(shared) variables
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
  SgInitializedName* parameter1=NULL; // the wrapper parameter
  SgVariableDeclaration*  local_var_decl  =  NULL;

  // handle OpenMP private variables/ or those which are neither live-in or live-out
  handlePrivateVariables(pSyms, body, private_remap);

  // --------------------------------------------------
  // for each parameters passed to the outlined function
  // They include parameters for regular shared variables and 
  // also the shared copies for firstprivate and reduction variables
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

    // step 1. Create parameters and insert it into the parameter list.
    // ----------------------------------------
    SgInitializedName* p_init_name = NULL;
    // Case 1: using a wrapper for all variables 
    //   two choices: array of pointers (default)  vs. structure 
    if (Outliner::useParameterWrapper)
    {
      if (i==syms.rbegin())
      {
        SgName var1_name = "__out_argv";
        // This is needed to support the pass-by-value semantics across different thread local stacks
        // In this situation, pointer dereferencing cannot be used to get the value 
        // of an inactive parent thread's local variables
        SgType* ptype= NULL; 
        if (Outliner::useStructureWrapper)
        {
          // To have strict type matching in C++ model
          // between the outlined function and the function pointer passed to the gomp runtime lib
          // we use void* for the parameter type
          #if 0 
          if (struct_decl != NULL)
            ptype = buildPointerType (struct_decl->get_type());
          else
          #endif  
            ptype = buildPointerType (buildVoidType());
        }
        else
          ptype= buildPointerType(buildPointerType(buildVoidType()));
        parameter1 = buildInitializedName(var1_name,ptype);
        appendArg(params,parameter1);
      }
      p_init_name = parameter1; // set the source parameter to the wrapper
    }
    else // case 3: use a parameter for each variable
       p_init_name = createOneFunctionParameter(i_name, readOnly, func); 

    // step 2. Create unpacking/unwrapping statements, also record variables to be replaced
    // ----------------------------------------
    bool isPointerDeref = false; 
    if (Outliner::temp_variable || Outliner::useStructureWrapper) 
    { // Check if the current variable belongs to the symbol set 
      //suitable for using pointer dereferencing
      const SgVariableSymbol* i_sym = isSgVariableSymbol(i_name->get_symbol_from_symbol_table ());
      ROSE_ASSERT(i_sym!=NULL);
      if ( pdSyms.find(i_sym)!=pdSyms.end())
        isPointerDeref = true;
    }  

    if (Outliner::enable_classic) 
    // classic methods use parameters directly, no unpacking is needed
    {
      if (!readOnly) 
      //read only variable should not have local variable declaration, using parameter directly
      // taking advantage of the same parameter names for readOnly variables
      // Let postprocessing to patch up symbols for them
      {
        // non-readonly variables need to be mapped to their parameters with different names (p__)
        // remapVarSyms() will use pointer dereferencing for all of them by default in C, 
        // this is enough to mimic the classic outlining work 
        recordSymRemap(*i,p_init_name, args_scope, sym_remap); 
      }
    } else 
    { // create unwrapping statements from parameters/ or the array parameter for pointers
      //if (SageInterface::is_Fortran_language())
      //  args_scope = NULL; // not sure about Fortran scope

      local_var_decl  = 
        createUnpackDecl (p_init_name, counter, isPointerDeref, i_name , struct_decl, body);
      ROSE_ASSERT (local_var_decl);
      prependStatement (local_var_decl,body);
#if 0 // moved to OmpSupport::transOmpVariables()     
      // also create a private copy for firstprivate and reduction variables
      // and transfer the value from the shared copy
      // // local_var_decl for regular (shared) firstprivate, reduction variables, 
      // int *_pp_sum1; 
      //   _pp_sum1 = ((int *)(__ompc_args[2]));
      //  // local_var_decl2 for firstprivate and reduction variables
      //  //  we avoid using copy constructor here
      //  int _p_sum1;  
      //  _p_sum1 = * _pp_sum1; // use a dedicated assignment statement instead
      if (fpSyms.find(*i)!=fpSyms.end() || fpSyms.find(*i)!=fpSyms.end())  
      {
        
      // firstprivate and reduction variables use the 2nd local declaration 
      // and use the private_remap instead of sym_remap
        local_var_decl2 = buildVariableDeclaration ("_p_"+name_str, i_name->get_type(), NULL, args_scope); 
        SageInterface::insertStatementAfter(local_var_decl, local_var_decl2);
        recordSymRemap (*i, local_var_decl2, args_scope, private_remap);
        SgExprStatement* assign_stmt = buildAssignStatement(buildVarRefExp(local_var_decl2), 
                             buildPointerDerefExp(buildVarRefExp(local_var_decl)));
        SageInterface::insertStatementAfter(local_var_decl2, assign_stmt);
      }
      else 
#endif        
        // regular and shared variables used the first local declaration
        recordSymRemap (*i, local_var_decl, args_scope, sym_remap);
      // transfer the value for firstprivate variables. 
      // TODO
    }

    // step 3. Create and insert companion re-pack statement in the end of the function body
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

  SgBasicBlock* func_body = func->get_definition()->get_body();

  //For OpenMP lowering, we have to have a void * parameter even if there is no need to pass any parameters 
  //in order to match the gomp runtime lib 's function prototype for function pointers
  SgFile* cur_file = getEnclosingFileNode(func);
  ROSE_ASSERT (cur_file != NULL);
  //if (cur_file->get_openmp_lowering () && ! SageInterface::is_Fortran_language())
  if (cur_file->get_openmp_lowering ())
  {
    if (syms.size() ==0)
    {
      SgName var1_name = "__out_argv";
      SgType* ptype= NULL; 
      // A dummy integer parameter for Fortran outlined function
      if (SageInterface::is_Fortran_language() )
      {
        var1_name = "out_argv";
        ptype = buildIntType();
        SgVariableDeclaration *var_decl = buildVariableDeclaration(var1_name,ptype, NULL, func_body);
        prependStatement(var_decl, func_body);

      }
      else
      {
        ptype = buildPointerType (buildVoidType());
        ROSE_ASSERT (Outliner::useStructureWrapper);
      }
      parameter1 = buildInitializedName(var1_name,ptype);
      appendArg(params,parameter1);
    }
  }

  // variable substitution 
  remapVarSyms (sym_remap, pdSyms, private_remap , func_body);
}

// =====================================================================

// DQ (2/25/2009): Modified function interface to pass "SgBasicBlock*" as not const parameter.
//! Create a function named 'func_name_str', with a parameter list from 'syms'
// pdSyms specifies symbols which must use pointer dereferencing if replaced during outlining, 
// only used when -rose:outline:temp_variable is used
// psyms are the symbols for OpenMP private variables, or dead variables (not live-in, not live-out)
SgFunctionDeclaration *
Outliner::generateFunction ( SgBasicBlock* s,
                                          const string& func_name_str,
                                          const ASTtools::VarSymSet_t& syms,
                                          const ASTtools::VarSymSet_t& pdSyms,
                                          const ASTtools::VarSymSet_t& psyms,
                                          SgClassDeclaration* struct_decl, 
                                          SgScopeStatement* scope)
{
  ROSE_ASSERT (s&&scope);
  ROSE_ASSERT(isSgGlobal(scope));
  // step 1: perform necessary liveness and side effect analysis, if requested.
  // ---------------------------------------------------------
  std::set< SgInitializedName *> liveIns, liveOuts;
  // Collect read-only variables of the outlining target
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

  //step 2. Create function skeleton, 'func'.
  // -----------------------------------------
  SgName func_name (func_name_str);
  SgFunctionParameterList *parameterList = buildFunctionParameterList();

  SgFunctionDeclaration* func = createFuncSkeleton (func_name,SgTypeVoid::createType (),parameterList, scope);
  ROSE_ASSERT (func);

  // Liao, 4/15/2009 , enforce C-bindings  for C++ outlined code
  // enable C code to call this outlined function
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

  //step 3. Create the function body
  // -----------------------------------------
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

  //step 4: variable handling, including: 
  // -----------------------------------------
  //   create parameters of the outlined functions
  //   add statements to unwrap the parameters
  //   add repacking statements if necessary
  //   replace variables to access to parameters, directly or indirectly
  variableHandling(syms, pdSyms, psyms, readOnlyVars, liveOuts, struct_decl, func);
  ROSE_ASSERT (func != NULL);

  // Retest this...
  ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());
  // printf ("After resetting the parent: func->get_definition() = %p func->get_definition()->get_body()->get_parent() = %p \n",func->get_definition(),func->get_definition()->get_body()->get_parent());
  //
  ROSE_ASSERT(scope->lookup_function_symbol(func->get_name()));
  return func;
}

// eof
