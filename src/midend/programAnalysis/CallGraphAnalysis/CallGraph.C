#include <sage3basic.h>

// DQ (10/16/2010): This is needed to use the HAVE_SQLITE3 macro.
// DQ (10/14/2010): This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#include "CallGraph.h"

#include "sageGeneric.h"

#ifndef _MSC_VER
#include <err.h>
#endif
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace std;
using namespace Rose;

/***************************************************
 * Get the vector of base types for the current type
 **************************************************/

static
SgType*
skipTypeAliases(SgType* ty)
{
  ROSE_ASSERT(ty);

  return ty->stripType( SgType::STRIP_TYPEDEF_TYPE );
}

namespace
{
  template <class SageNode>
  SgType* genericUnderType(SageNode* ty)
  {
    ROSE_ASSERT(ty);

    return ty->get_base_type();
  }

  SgType* getUnderType(SgType* ty)                { return NULL /* base case */; }
  SgType* getUnderType(SgModifierType* ty)        { return genericUnderType(ty); }
  SgType* getUnderType(SgPointerType* ty)         { return genericUnderType(ty); }
  SgType* getUnderType(SgReferenceType* ty)       { return genericUnderType(ty); }
  SgType* getUnderType(SgRvalueReferenceType* ty) { return genericUnderType(ty); }
  SgType* getUnderType(SgArrayType* ty)           { return genericUnderType(ty); }
  SgType* getUnderType(SgTypedefType* ty)         { return genericUnderType(ty); }
}

static
std::vector<SgType*>
get_type_vector(SgType* currentType )
{
  std::vector<SgType*> returnVector;

  while (true)
  {
    ROSE_ASSERT(currentType);

    returnVector.push_back(currentType);

    if (SgModifierType* modType = isSgModifierType(currentType))
    {
      currentType = getUnderType(modType);
    }
    else if (SgReferenceType* refType = isSgReferenceType(currentType))
    {
      currentType = getUnderType(refType);
    }
    else if (SgRvalueReferenceType* rvRefType = isSgRvalueReferenceType(currentType))
    {
      currentType = getUnderType(rvRefType);
    }
    else if (SgPointerType* pointType = isSgPointerType(currentType))
    {
      currentType = getUnderType(pointType);
    }
    else if (SgArrayType* arrayType = isSgArrayType(currentType))
    {
      currentType = getUnderType(arrayType);
    }
    else if (SgTypedefType* typedefType = isSgTypedefType(currentType))
    {
      // DQ (6/21/2005): Added support for typedef types to be uncovered by findBaseType()

      currentType = getUnderType(typedefType);
      returnVector.pop_back(); // PP (29/01/20) typedef types should be used for comparisons
    }
    else {
      // \todo PP: templated types, using aliases

      // Exit the while(true){} loop!
      break;
    }
  }

  return returnVector;
};

static
bool is_functions_types_equal(SgFunctionType* f1, SgFunctionType* f2);


static
bool typeEquality(SgType*, SgType*)
{
  return true;
}

static
bool typeEquality(SgNamedType* type1, SgNamedType* type2)
{
  ROSE_ASSERT(type1 && type2);

  SgDeclarationStatement& decl1 = SG_DEREF(type1->get_declaration());
  SgDeclarationStatement& decl2 = SG_DEREF(type2->get_declaration());

  return decl1.get_firstNondefiningDeclaration() == decl2.get_firstNondefiningDeclaration();
}

static
bool typeEquality(SgFunctionType* type1, SgFunctionType* type2)
{
  ROSE_ASSERT(type1 && type2);

  return is_functions_types_equal(type1, type2);
}

static
bool typeEquality(SgModifierType* type1, SgModifierType* type2)
{
  ROSE_ASSERT(type1 && type2);

  bool            types_are_equal = true;
  SgTypeModifier& typeModifier1 = type1->get_typeModifier();
  SgTypeModifier& typeModifier2 = type1->get_typeModifier();

  if( typeModifier1.get_modifierVector () != typeModifier2.get_modifierVector ()   )
    types_are_equal = false;

  if( typeModifier1.get_upcModifier().get_modifier () !=
      typeModifier2.get_upcModifier().get_modifier ()
    )
    types_are_equal = false;

  if( typeModifier1.get_constVolatileModifier().get_modifier () !=
      typeModifier2.get_constVolatileModifier().get_modifier ()
    )
    types_are_equal = false;

  if( typeModifier1.get_elaboratedTypeModifier().get_modifier () !=
      typeModifier2.get_elaboratedTypeModifier().get_modifier ()
    )
    types_are_equal = false;

  return types_are_equal;
}

static
bool is_types_equal( SgType* t1, SgType* t2 )
{
  if( t1 == t2 )
    return true;

  bool types_are_equal = true;
  std::vector<SgType*> f1_vec = get_type_vector( t1  );
  std::vector<SgType*> f2_vec = get_type_vector( t2  );

  if( f1_vec.size() == f2_vec.size() )
  {
    for( size_t i = 0; i < f1_vec.size(); i++ )
    {
      if( f1_vec[i]->variantT() == f2_vec[i]->variantT() )
      {
        //The named types do not point to the same declaration
        if( isSgNamedType(f1_vec[i]) != NULL )
        {
          if ( !typeEquality(isSgNamedType(f1_vec[i]), isSgNamedType(f2_vec[i])) )
            types_are_equal = false;
        }

        if(  isSgModifierType( f1_vec[i] ) != NULL  )
        {
          if (!typeEquality(isSgModifierType( f1_vec[i] ), isSgModifierType( f2_vec[i] )))
            types_are_equal = false;
        }

        //Function types are not the same
        if( isSgFunctionType( f1_vec[i] ) != NULL )
        {
          if ( !typeEquality(isSgFunctionType(f1_vec[i]), isSgFunctionType(f2_vec[i])) )
            types_are_equal = false;
        }
      }else{
        //Variant is different
        types_are_equal = false;
      }

      if( types_are_equal == false )
        break;

    }

  }else{
    //The size of the type vectors are not the same
    types_are_equal = false;
  }

  return types_are_equal;
};

static
bool is_functions_types_equal(SgFunctionType* f1, SgFunctionType* f2)
{
  bool functions_are_equal = false;

  //Optimization: Function type objects are the same for functions that
  //have exactly the same signature
  if( f1 == f2 )
    return true;

  //See if the function types match
  if(  is_types_equal( f1->get_return_type(), f2->get_return_type() ) )
  {
    SgTypePtrList& args_f1 = f1->get_arguments();
    SgTypePtrList& args_f2 = f2->get_arguments();

    //See if the arguments match

    if( args_f1.size() == args_f2.size() )
    {
      functions_are_equal = true;

      for(size_t i = 0; i < args_f1.size(); i++)
      {
        if( is_types_equal( args_f1[i], args_f2[i] ) == false  )
        {
          functions_are_equal = false;
          break;
        }

      }
    }// Different number of arguments
  }

  //std::cout << "is_functions_types_equal: " << f1->unparseToString() << " " << f2->unparseToString() <<  ( functions_are_equal == true ? " true " : " false " ) << std::endl;

  return functions_are_equal;
}


struct CovarianceChecker : sg::DispatchHandler<bool>
{
    explicit
    CovarianceChecker(SgType& baseType, ClassHierarchyWrapper* hierarchy = NULL)
    : base(&baseType), chw(hierarchy)
    {}

    template <class T>
    static
    bool typeChk(T& derived, T& base, ClassHierarchyWrapper* chw)
    {
      return ::typeEquality(&derived, &base);
    }

    static
    bool typeChk(SgNamedType& derived, SgNamedType& base, ClassHierarchyWrapper* chw)
    {
      typedef ClassHierarchyWrapper::ClassDefSet ClassDefSet;

      const bool         sameTypes = ::typeEquality(&derived, &base);
      if (!chw || sameTypes) return sameTypes;

      SgClassType*       derivedClass = isSgClassType(&derived);
      if (!derivedClass) return false;

      SgClassType*       baseClass = isSgClassType(&base);
      if (!baseClass) return false;

      SgClassDeclaration& derivedDcl0 = SG_ASSERT_TYPE(SgClassDeclaration, SG_DEREF(derivedClass->get_declaration()));
      SgClassDeclaration& baseDcl0    = SG_ASSERT_TYPE(SgClassDeclaration, SG_DEREF(baseClass->get_declaration()));
      SgClassDeclaration* derivedDcl  = isSgClassDeclaration(derivedDcl0.get_definingDeclaration());
      SgClassDeclaration* baseDcl     = isSgClassDeclaration(baseDcl0.get_definingDeclaration());
      if (!derivedDcl || !baseDcl) return false;

      SgClassDefinition*  derivedDef  = derivedDcl->get_definition();
      SgClassDefinition*  baseDef     = baseDcl->get_definition();
      const ClassDefSet& ancestors    = chw->getAncestorClasses(derivedDef);

      return ancestors.find(baseDef) != ancestors.end();
    }

    template <class T>
    static
    bool underChk(T& derived, T& base, ClassHierarchyWrapper* chw)
    {
      SgType* derived_base = getUnderType(&derived);

      // if there is no underlying type and everything matched up to
      //   here, we have found a winner.
      if (!derived_base) return true;

      return isCovariantType(derived_base, &base, chw);
    }

    ReturnType
    descend(SgType* ty) const
    {
      return sg::dispatch(*this, ty);
    }

    /// generic template routine to check for covariance
    /// if @chw is null, the check tests for strict equality
    template <class T>
    bool check(T& derivedTy, ClassHierarchyWrapper* nextChw = NULL)
    {
      base = skipTypeAliases(base);

      // symmetric check for strict type equality
      if (typeid(derivedTy) != typeid(*base))
        return false;

      T& baseTy = SG_ASSERT_TYPE(T, *base);

      if (&derivedTy == &baseTy)
        return true;

      return (  typeChk(derivedTy, baseTy, nextChw)
             && underChk(derivedTy, baseTy, nextChw)
             );
    }

    void handle(SgNode& derived)                  { SG_UNEXPECTED_NODE(derived); }

    void handle(SgType& derived)                  { res = check(derived); }

    // skip typedefs
    void handle(SgTypedefType& derived)           { res = descend(getUnderType(&derived)); }

    // check equality of underlying types
    // @{
    void handle(SgPointerType& derived)           { res = check(derived); }
    void handle(SgArrayType& derived)             { res = check(derived); }
    // @}

    // covariance is only maintained through classes and modifiers
    // @{
    void handle(SgNamedType& derived)             { res = check(derived, chw); }
    void handle(SgModifierType& derived)          { res = check(derived, chw); }
    // @}

    // should have been removed by PolymorphicRootsFinder
    // @{
    void handle(SgReferenceType& derived)         { SG_UNEXPECTED_NODE(derived); }
    void handle(SgRvalueReferenceType& derived)   { SG_UNEXPECTED_NODE(derived); }
    // @}

  private:
    SgType*                base;
    ClassHierarchyWrapper* chw;
};

struct PolymorphicRootsFinder : sg::DispatchHandler< std::pair<SgType*, SgType*> >
{
    explicit
    PolymorphicRootsFinder(SgType& baseType)
    : base(&baseType)
    {}

    static
    bool typeChk(SgType& derived, SgType& base)
    {
      return true;
    }

    static
    bool typeChk(SgModifierType& derived, SgModifierType& base)
    {
      return ::typeEquality(&derived, &base);
    }

    ReturnType
    descend(SgType* ty) const
    {
      return sg::dispatch(*this, ty);
    }

    /// generic template routine to check for covariance
    /// if @chw is null, the check tests for strict equality
    template <class T>
    ReturnType
    check(T& derivedTy)
    {
      ReturnType res(NULL, NULL);

      base = skipTypeAliases(base);

      // symmetric check for strict type equality
      if (typeid(derivedTy) != typeid(*base))
        return res;

      T& baseTy = SG_ASSERT_TYPE(T, *base);

      if (typeChk(derivedTy, baseTy))
      {
        res = ReturnType(getUnderType(&derivedTy), getUnderType(&baseTy));
      }

      return res;
    }

    void handle(SgNode& derived)                  { SG_UNEXPECTED_NODE(derived); }

    void handle(SgType& derived)                  {}

    // skip typedefs
    void handle(SgTypedefType& derived)           { res = descend(getUnderType(&derived)); }

    // the modifiers must equal
    void handle(SgModifierType& derived)          { res = check(derived); }

    // polymorphic root types (must also equal)
    // @{
    void handle(SgReferenceType& derived)         { res = check(derived); }
    void handle(SgRvalueReferenceType& derived)   { res = check(derived); }
    void handle(SgPointerType& derived)           { res = check(derived); }
    // @}
  private:
    SgType* base;
};


/// tests if @derived is a covariant type of @base
/// \param derived a non-null type
/// \param base    a non-null type
/// \param chw     the "unrolled" class hierarchy information
/// \brief if chw is NULL, strict type equality is checked
static
bool
isCovariantType(SgType* derived, SgType* base, ClassHierarchyWrapper* chw)
{
  // find polymorphic root (e.g., reference, pointer)
  std::pair<SgType*, SgType*> rootTypes = sg::dispatch(PolymorphicRootsFinder(SG_DEREF(base)), derived);

  if (!rootTypes.first)
    return sg::dispatch(CovarianceChecker(SG_DEREF(base)), derived);

  // test if the roots are covariant
  return sg::dispatch(CovarianceChecker(SG_DEREF(rootTypes.second), chw), rootTypes.first);
}


/// returns @derived is an overriding type of @base
/// \param derived a function type of an overrider candidate
/// \param base    a function type
/// \param chw     a class hierarchy
/// \pre derived != NULL && base != NULL
/// \details
///    isOverridingType checks if derived overrides base, meaning
///    @derived's arguments must equal @base's argument, and @derived's
///    return type may be covariabt with respect to @base's return type.
static
bool
isOverridingType( SgMemberFunctionType* derived,
                  SgMemberFunctionType* base,
                  ClassHierarchyWrapper* chw
                )
{
  ROSE_ASSERT(derived && base);

  if (derived == base) return true;

  if (  derived->get_mfunc_specifier() != base->get_mfunc_specifier()
     || derived->get_ref_qualifiers()  != base->get_ref_qualifiers()
     || derived->get_has_ellipses()    != base->get_has_ellipses()
     )
  {
    return false;
  }

  if (!isCovariantType(derived->get_return_type(), base->get_return_type(), chw))
    return false;

  SgTypePtrList& args_derived = derived->get_arguments();
  SgTypePtrList& args_base    = base->get_arguments();

  // See if the arguments match
  if (args_derived.size() != args_base.size())
    return false;

  SgTypePtrList::iterator derived_end = args_derived.end();

  return std::mismatch( args_derived.begin(), derived_end,
                        args_base.begin(),
                        is_types_equal
                      ).first == derived_end;
}

/// tests if derived overrides base
static
bool isOverridingFunction( SgMemberFunctionDeclaration* candidate,
                           SgMemberFunctionDeclaration* member,
                           ClassHierarchyWrapper*       chw
                         )
{
  ROSE_ASSERT(candidate && member);

  return (  candidate->get_name() == member->get_name()
         && isOverridingType( isSgMemberFunctionType(candidate->get_type()),
                              isSgMemberFunctionType(member->get_type()),
                              chw
                            )
         );
}

SgFunctionDeclaration*
is_function_exists(SgClassDefinition *cls, SgMemberFunctionDeclaration *memberFunctionDeclaration)
{
  SgFunctionDeclaration *resultDecl = NULL;
  string f1 = memberFunctionDeclaration->get_name().getString();
  string f2;

  SgDeclarationStatementPtrList &clsMembers = cls->get_members();
  for ( SgDeclarationStatementPtrList::iterator it_cls_mb = clsMembers.begin(); it_cls_mb != clsMembers.end(); it_cls_mb++ )
  {
    SgMemberFunctionDeclaration *cls_mb_decl = isSgMemberFunctionDeclaration( *it_cls_mb );
    if (cls_mb_decl == NULL) continue;

    ROSE_ASSERT(cls_mb_decl != NULL);
    SgMemberFunctionType* funcType1 = isSgMemberFunctionType(memberFunctionDeclaration->get_type());
    SgMemberFunctionType* funcType2 = isSgMemberFunctionType(cls_mb_decl->get_type());
    f2 = cls_mb_decl->get_name().getString();

    if(f1 != f2) continue;
    if (funcType1 == NULL || funcType2 == NULL) continue;
    if ( is_functions_types_equal(funcType1, funcType2) )
    {
      SgMemberFunctionDeclaration *nonDefDecl =
        isSgMemberFunctionDeclaration( cls_mb_decl->get_firstNondefiningDeclaration() );
      SgMemberFunctionDeclaration *defDecl =
        isSgMemberFunctionDeclaration( cls_mb_decl->get_definingDeclaration() );

      // ROSE_ASSERT ( (!nonDefDecl && defDecl == cls_mb_decl) || (nonDefDecl == cls_mb_decl && nonDefDecl) );

      resultDecl = (nonDefDecl) ? nonDefDecl : defDecl;
      ROSE_ASSERT ( resultDecl );
      if ( !( resultDecl->get_functionModifier().isPureVirtual() ) )
      {
          return resultDecl;
        //resultDecl = functionDeclarationInClass;
        //functionList.push_back( functionDeclarationInClass );
      }
    }
  }

  ROSE_ASSERT(!isSgTemplateFunctionDeclaration(resultDecl));
  return resultDecl;
}

bool
dummyFilter::operator() (SgFunctionDeclaration* node) const{
  assert(!isSgTemplateFunctionDeclaration(node));
  return true;
};

bool
builtinFilter::operator() (SgFunctionDeclaration* funcDecl) const
{
  assert(!isSgTemplateFunctionDeclaration(funcDecl));
  bool returnValue = true;
  ROSE_ASSERT(funcDecl != NULL);
  string filename = funcDecl->get_file_info()->get_filename();
  std::string func_name = funcDecl->get_name().getString();
  string stripped_file_name = StringUtility::stripPathFromFileName(filename);
  //string::size_type loc;

  //Filter out functions from the ROSE preinclude header file
  if(filename.find("rose_edg_required_macros_and_functions")!=string::npos)
    returnValue = false;
  //Filter out compiler generated functions
  else if(funcDecl->get_file_info()->isCompilerGenerated()==true)
    returnValue=false;
  //Filter out compiler generated functions
  else if(funcDecl->get_file_info()->isFrontendSpecific()==true)
    returnValue=false;
  // filter out other built in functions
  //      else if( func_name.find ("__",0)== 0);
  //         returnValue = false;
  // _IO_getc _IO_putc _IO_feof, etc.
  //loc = func_name.find ("_IO_",0);
  //if (loc == 0 ) returnValue = false;

  // skip functions from standard system headers
  // TODO Need more rigid check
  else  if (
      stripped_file_name==string("assert.h")||
      stripped_file_name==string("complex.h")||
      stripped_file_name==string("ctype.h")||
      stripped_file_name==string("errno.h")||
      stripped_file_name==string("float.h")||

      stripped_file_name==string("limits.h")||
      stripped_file_name==string("locale.h")||
      stripped_file_name==string("math.h")||
      stripped_file_name==string("setjmp.h")||
      stripped_file_name==string("signal.h")||

      stripped_file_name==string("stdarg.h")||
      stripped_file_name==string("stddef.h")||

      stripped_file_name==string("stdio.h")||
      stripped_file_name==string("stdlib.h")||
      stripped_file_name==string("string.h")||
      stripped_file_name==string("time.h")||

// GCC specific ???
      stripped_file_name==string("libio.h")||
      stripped_file_name==string("select.h")||
      stripped_file_name==string("mathcalls.h")
      )
    returnValue=false;
 if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
    cout<<"Debug: CallGraph.C ... "<<func_name << " from file:"<<stripped_file_name<<" predicate function returns: "<<returnValue<<endl;
  return returnValue;
}


  bool
FunctionData::isDefined ()
{
  return hasDefinition;
}


CallGraphBuilder::CallGraphBuilder( SgProject *proj)
{
  project = proj;
  graph = NULL;
}

  SgIncidenceDirectedGraph*
CallGraphBuilder::getGraph()
{
  return graph;
}




Rose_STL_Container<SgFunctionDeclaration*>
CallTargetSet::solveFunctionPointerCallsFunctional(SgNode* node, SgFunctionType* functionType )
{
  Rose_STL_Container<SgFunctionDeclaration*> functionList;

  SgFunctionDeclaration* fctDecl = isSgFunctionDeclaration(node);
  ROSE_ASSERT( fctDecl != NULL );
  assert(!isSgTemplateFunctionDeclaration(fctDecl));
  //if ( functionType == fctDecl->get_type() )
  //Find all function declarations which is both first non-defining declaration and
  //has a mangled name which is equal to the mangled name of 'functionType'
  if( functionType->get_mangled().getString() == fctDecl->get_type()->get_mangled().getString() )
  {
    //ROSE_ASSERT( functionType->get_mangled().getString() == fctDecl->get_mangled().getString() );

    SgFunctionDeclaration *nonDefDecl =
      isSgFunctionDeclaration( fctDecl->get_firstNondefiningDeclaration() );
    assert(!isSgTemplateFunctionDeclaration(nonDefDecl));

    //The ROSE AST normalizes functions so that there should be a nondef function decl for
    //every function
    //ROSE_ASSERT( nonDefDecl != NULL );
    if( fctDecl == nonDefDecl )
      functionList.push_back( nonDefDecl );
    else
      functionList.push_back( fctDecl );
  }//else
  //ROSE_ASSERT( functionType->get_mangled().getString() != fctDecl->get_type()->get_mangled().getString() );

  return functionList;
}



std::vector<SgFunctionDeclaration*>
CallTargetSet::solveFunctionPointerCall( SgPointerDerefExp *pointerDerefExp, SgProject *project )
{
  SgFunctionDeclarationPtrList functionList;

  SgFunctionType *fctType = isSgFunctionType( pointerDerefExp->get_type()->findBaseType() );
  ROSE_ASSERT ( fctType );
  ROSE_ASSERT ( project );
  // SgUnparse_Info ui;
  // string type1str = fctType->get_mangled( ui ).str();
  string type1str = fctType->get_mangled().str();
  // cout << "Return type of function pointer " << type1str << "\n";

  // cout << " Line: " << pointerDerefExp->get_file_info()->get_filenameString() <<
  //  " l" << pointerDerefExp->get_file_info()->get_line() <<
  //  " c" << pointerDerefExp->get_file_info()->get_col()  << std::endl;
  // getting all possible functions with the same type
  // DQ (1/31/2006): Changed name and made global function type symbol table a static data member.
  // SgType *ty = Sgfunc_type_table.lookup_function_type( fctType->get_mangled( ui ) );
  ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
  // SgType *ty = SgNode::get_globalFunctionTypeTable()->lookup_function_type( fctType->get_mangled( ui ) );
  // ROSE_ASSERT ( ty->get_mangled( ui ) == type1str );

  // if there are multiple forward declarations of the same function
  // there will be multiple nodes in the AST containing them
  // but just one link in the call graph
  //  list<SgNode *> fctDeclarationList = NodeQuery::querySubTree( project, V_SgFunctionDeclaration );
  //AS (09/23/06) Query the memory pool instead of subtree of project
  //AS (10/2/06)  Modified query to only query for functions or function templates
  //VariantVector vv = V_SgFunctionDeclaration;
  VariantVector vv;
  vv.push_back(V_SgFunctionDeclaration);
  vv.push_back(V_SgTemplateInstantiationFunctionDecl);

  functionList =  AstQueryNamespace::queryMemoryPool(std::bind2nd(std::ptr_fun(solveFunctionPointerCallsFunctional), fctType), &vv );

  return functionList;
}

std::vector<SgFunctionDeclaration*>
CallTargetSet::solveMemberFunctionPointerCall(SgExpression *functionExp, ClassHierarchyWrapper *classHierarchy)
{
    SgBinaryOp *binaryExp = isSgBinaryOp(functionExp);
    ROSE_ASSERT(isSgArrowStarOp(binaryExp) || isSgDotStarOp(binaryExp));

    SgExpression *left = NULL, *right = NULL;
    SgClassType *classType = NULL;
    SgClassDefinition *classDefinition = NULL;
    std::vector<SgFunctionDeclaration*> functionList;
    SgMemberFunctionType *memberFunctionType = NULL;

    left = binaryExp->get_lhs_operand();
    right = binaryExp->get_rhs_operand();
    ROSE_ASSERT(left->get_type());
    
    SgType* leftBase = left->get_type()->findBaseType();

    // left side of the expression should have class type (but it might have SgTemplateType which we should ignore since that
    // means we're trying to do call graph analysis inside a class template, which doesn't make much sense.)
    if (isSgTemplateType(leftBase) || isSgNonrealType(leftBase))
    {
        ROSE_ASSERT(functionList.empty());
        return functionList;
    }
        
    classType = isSgClassType(leftBase);
    ROSE_ASSERT(classType != NULL);
    ROSE_ASSERT(classType->get_declaration() != NULL);
    ROSE_ASSERT(classType->get_declaration()->get_definingDeclaration() != NULL);
    
    SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classType->get_declaration()->get_definingDeclaration());
    ROSE_ASSERT(definingClassDeclaration != NULL);
    
    classDefinition = definingClassDeclaration->get_definition();
    ROSE_ASSERT(classDefinition != NULL);

    // right side of the expression should have member function type
    memberFunctionType = isSgMemberFunctionType(right->get_type()->findBaseType());
    ROSE_ASSERT(memberFunctionType);

    SgDeclarationStatementPtrList &allMembers = classDefinition->get_members();
    for (SgDeclarationStatementPtrList::iterator it = allMembers.begin(); it != allMembers.end(); it++)
    {
        SgMemberFunctionDeclaration *memberFunctionDeclaration = isSgMemberFunctionDeclaration(*it);
        if (memberFunctionDeclaration)
        {
            SgMemberFunctionDeclaration *nonDefDecl = isSgMemberFunctionDeclaration(memberFunctionDeclaration->get_firstNondefiningDeclaration());
            if (nonDefDecl)
                memberFunctionDeclaration = nonDefDecl;

            //FIXME: Make this use the is_functions_types_equal function
            if (memberFunctionDeclaration->get_type()->unparseToString() == memberFunctionType->unparseToString())
            {
                if (!(memberFunctionDeclaration->get_functionModifier().isPureVirtual()))
                {
                    functionList.push_back(memberFunctionDeclaration);
                }
            }

            // for virtual functions in polymorphic calls, we need to search down in the hierarchy of classes
            // and retrieve all declarations of member functions with the same type
            if ((memberFunctionDeclaration->get_functionModifier().isVirtual() ||
                    memberFunctionDeclaration->get_functionModifier().isPureVirtual()) && !isSgThisExp(left))
            {
                const ClassHierarchyWrapper::ClassDefSet& subclasses = classHierarchy->getSubclasses(classDefinition);
                //cout << "Virtual function " << memberFunctionDeclaration->get_mangled_name().str() << "\n";
                for (ClassHierarchyWrapper::ClassDefSet::const_iterator it_cls = subclasses.begin(); it_cls != subclasses.end(); it_cls++)
                {
                    SgClassDefinition *cls = isSgClassDefinition(*it_cls);
                    SgDeclarationStatementPtrList &clsMembers = cls->get_members();

                    for (SgDeclarationStatementPtrList::iterator it_cls_mb = clsMembers.begin(); it_cls_mb != clsMembers.end(); it_cls_mb++)
                    {
                        SgMemberFunctionDeclaration *cls_mb_decl = isSgMemberFunctionDeclaration(*it_cls_mb);

                     // TV (10/26/2018): cannot expect that all class members would be methods FIXME ROSE-1487
                        if (cls_mb_decl == NULL) continue;

                        if (is_functions_types_equal(isSgMemberFunctionType(memberFunctionDeclaration->get_type()), isSgMemberFunctionType(cls_mb_decl->get_type())))
                        {
                            SgMemberFunctionDeclaration *nonDefDecl = isSgMemberFunctionDeclaration(cls_mb_decl->get_firstNondefiningDeclaration());
                            SgMemberFunctionDeclaration *defDecl = isSgMemberFunctionDeclaration(cls_mb_decl->get_definingDeclaration());

                         // TV (10/26/2018): this case happens in Doxygen, not sure it is valid... FIXME ROSE-1487
                         // ROSE_ASSERT((!nonDefDecl && defDecl == cls_mb_decl) || (nonDefDecl == cls_mb_decl && nonDefDecl));

                            if (nonDefDecl)
                            {
                                if (!(nonDefDecl->get_functionModifier().isPureVirtual()) && nonDefDecl->get_functionModifier().isVirtual())
                                {
                                    functionList.push_back(nonDefDecl);
                                }
                            }
                            else
                                if (!(defDecl->get_functionModifier().isPureVirtual()) && defDecl->get_functionModifier().isVirtual())
                            {
                                functionList.push_back(defDecl);
                            }
                        }
                    }
                }
            }
        }
    }

    //cout << "Function list size: " << functionList.size() << "\n";
    return functionList;
}

static
bool isPureVirtual(SgMemberFunctionDeclaration* dcl)
{
  ROSE_ASSERT(dcl);

  return dcl->get_functionModifier().isPureVirtual();
}

static
void solveVirtualFunctionCall( SgClassType* crtClass,
                               ClassHierarchyWrapper* classHierarchy,
                               SgMemberFunctionDeclaration* memberFunctionDeclaration,
                               SgMemberFunctionDeclaration* functionDeclarationInClass,
                               bool includePureVirtualFunc,
                               std::vector<SgFunctionDeclaration*>& result
                             )
{
  ROSE_ASSERT(classHierarchy && memberFunctionDeclaration && functionDeclarationInClass);

  // If it's not pure virtual then the current function declaration is a candidate function to be called
  if (includePureVirtualFunc || !isPureVirtual(functionDeclarationInClass))
    result.push_back( functionDeclarationInClass );

  // Search down the class hierarchy to get all redeclarations of the current member function which may be the ones being
  // called via polymorphism.
  SgClassDefinition* crtClsDef = NULL;

  // selecting the root of the hierarchy
  if (crtClass)
  {
    SgClassDeclaration* tmp = isSgClassDeclaration(crtClass->get_declaration());
    ROSE_ASSERT(tmp);
    SgClassDeclaration* tmp2 = isSgClassDeclaration(tmp->get_definingDeclaration());
    ROSE_ASSERT(tmp2);

    crtClsDef = tmp2->get_definition();
    ROSE_ASSERT(crtClsDef);
  }
  else
  {
    crtClsDef = isSgClassDefinition(memberFunctionDeclaration->get_scope());
    ROSE_ASSERT(crtClsDef);
  }

  // For virtual functions, we need to search down in the hierarchy of classes and retrieve all declarations of member
  // functions with the same name and type.  Names are not important for destructors.
  const ClassHierarchyWrapper::ClassDefSet& subclasses = classHierarchy->getSubclasses(crtClsDef);

  std::string f1 = memberFunctionDeclaration->get_mangled_name().str();
  ROSE_ASSERT(!memberFunctionDeclaration->get_name().getString().empty());

  const bool isDestructor1 = '~' == memberFunctionDeclaration->get_name().getString()[0];

  for (ClassHierarchyWrapper::ClassDefSet::const_iterator sci=subclasses.begin(); sci!=subclasses.end(); ++sci)
  {
    SgClassDefinition* cls = isSgClassDefinition(*sci);
    SgDeclarationStatementPtrList& clsMembers = cls->get_members();

    for (SgDeclarationStatementPtrList::iterator cmi=clsMembers.begin(); cmi!=clsMembers.end(); ++cmi)
    {
      SgMemberFunctionDeclaration *cls_mb_decl = isSgMemberFunctionDeclaration(*cmi);
      if (cls_mb_decl == NULL)
        continue;

      ROSE_ASSERT(!cls_mb_decl->get_name().getString().empty());

      const bool isDestructor2 = '~' == cls_mb_decl->get_name().getString()[0];
      const bool keep = (  (isDestructor1 && isDestructor2)
                        || isOverridingFunction(cls_mb_decl, memberFunctionDeclaration, classHierarchy)
                        );

      if (keep)
      {
        SgMemberFunctionDeclaration* nonDefDecl =
            isSgMemberFunctionDeclaration(cls_mb_decl->get_firstNondefiningDeclaration());
        SgMemberFunctionDeclaration* defDecl =
            isSgMemberFunctionDeclaration(cls_mb_decl->get_definingDeclaration());

        // MD 2010/07/08 defDecl might be NULL
        SgMemberFunctionDeclaration* candidate = nonDefDecl ? nonDefDecl : defDecl;
        ROSE_ASSERT(candidate);

        if (includePureVirtualFunc || !isPureVirtual(candidate))
          result.push_back(candidate);
      }
    }
  }
}

std::vector<SgFunctionDeclaration*>
CallTargetSet::solveMemberFunctionCall(SgClassType *crtClass, ClassHierarchyWrapper* classHierarchy,
                                       SgMemberFunctionDeclaration* memberFunctionDeclaration, bool polymorphic,
                                       bool includePureVirtualFunc)
{
    ROSE_ASSERT(memberFunctionDeclaration);

    std::vector<SgFunctionDeclaration*> functionList;

    SgMemberFunctionDeclaration *functionDeclarationInClass = NULL;
    if (SgDeclarationStatement *nonDefDeclInClass = memberFunctionDeclaration->get_firstNondefiningDeclaration()) {
        // memberFunctionDeclaration is outside the class
        functionDeclarationInClass = isSgMemberFunctionDeclaration(nonDefDeclInClass);
    } else {
        // In class declaration, since there is no non-defining declaration
        functionDeclarationInClass = memberFunctionDeclaration;
    }
    ROSE_ASSERT(functionDeclarationInClass);

    // We need the inclass declaration so we can determine if it is a virtual function
    if (functionDeclarationInClass->get_functionModifier().isVirtual() && polymorphic) {
        // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
        // SgFunctionDefinition *functionDefinition = NULL;

        // SgMemberFunctionDeclaration* memberFunctionDefDeclaration =
        //     isSgMemberFunctionDeclaration(memberFunctionDeclaration->get_definingDeclaration());

        // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
        // if (memberFunctionDefDeclaration != NULL) {
        //     functionDefinition = memberFunctionDefDeclaration->get_definition();
        // }
        solveVirtualFunctionCall( crtClass,
                                  classHierarchy,
                                  memberFunctionDeclaration,
                                  functionDeclarationInClass,
                                  includePureVirtualFunc,
                                  functionList
                                );
    } else {
        // Non virtual (standard) member function or call not polymorphic (or both)
        // Always pushing the in-class declaration, so we need to find that one
        SgDeclarationStatement *nonDefDeclInClass = memberFunctionDeclaration->get_firstNondefiningDeclaration();
        SgMemberFunctionDeclaration *functionDeclarationInClass = NULL;
        if (nonDefDeclInClass) {
            // memberFunctionDeclaration is outside the class
            functionDeclarationInClass = isSgMemberFunctionDeclaration(nonDefDeclInClass);
        } else {
            // In class declaration, since there is no non-defining declaration
            functionDeclarationInClass = memberFunctionDeclaration;
        }
        ROSE_ASSERT(functionDeclarationInClass);
        functionList.push_back(functionDeclarationInClass);
    }
    return functionList;
}



Rose_STL_Container<SgFunctionDeclaration*> solveFunctionPointerCallsFunctional(SgNode* node, SgFunctionType* functionType )
{
  Rose_STL_Container<SgFunctionDeclaration*> functionList;

  SgFunctionDeclaration* fctDecl = isSgFunctionDeclaration(node);
  ROSE_ASSERT( fctDecl != NULL );
  assert(!isSgTemplateFunctionDeclaration(fctDecl));
  //if ( functionType == fctDecl->get_type() )
  //Find all function declarations which is both first non-defining declaration and
  //has a mangled name which is equal to the mangled name of 'functionType'
  if( functionType->get_mangled().getString() == fctDecl->get_type()->get_mangled().getString() )
  {
    //ROSE_ASSERT( functionType->get_mangled().getString() == fctDecl->get_mangled().getString() );

    SgFunctionDeclaration *nonDefDecl =
      isSgFunctionDeclaration( fctDecl->get_firstNondefiningDeclaration() );

    //The ROSE AST normalizes functions so that there should be a nondef function decl for
    //every function
    ROSE_ASSERT( nonDefDecl != NULL );
    assert(!isSgTemplateFunctionDeclaration(nonDefDecl));
    if( fctDecl == nonDefDecl )
      functionList.push_back( nonDefDecl );
  }//else
  //ROSE_ASSERT( functionType->get_mangled().getString() != fctDecl->get_type()->get_mangled().getString() );

  return functionList;
}

/**
 * This function determines all the constructors called in a constructor 
 * initializer.  For example: 
 *   Bar::Bar() : foo() {} 
 *   In this case, we need to list foo() as having been called, and the constructors
 *   for all of foo's BaseClasses.  (These are returned as SgFunctionDeclarations, in 
 *   the props vector)
 **/
std::vector<SgFunctionDeclaration*>
CallTargetSet::solveConstructorInitializer(SgConstructorInitializer* sgCtorInit)
{
    //FIXME: There is a bug in this function.
    //Consider the inheritance hierarchy A -> B -> C (C inherits from B, B inherits from A).
    //Let C have an explicit constructor, without explictly calling B's constructor. We will only return the constructor for C


    std::vector<SgFunctionDeclaration*> props;
    SgMemberFunctionDeclaration* memFunDecl = sgCtorInit->get_declaration();

    //It's possible to have a null constructor declaration, in case of compiler-generated
    //default constructors.
    if (memFunDecl == NULL || memFunDecl->get_file_info()->isCompilerGenerated())
    {
        //If there are superclasses, the constructors for those classes may have been called. We need to return them

        //Sometimes constructor initializers appear for primitive types. (e.g. x() in a constructor initializer list)
        if (sgCtorInit->get_class_decl() != NULL)
        {
            //The worklist contains classes that are initialized through compiler-generated default constructors
            vector<SgClassDeclaration*> worklist;
            worklist.push_back(sgCtorInit->get_class_decl());

            while (!worklist.empty())
            {
                SgClassDeclaration* currClassDecl = worklist.back();
                worklist.pop_back();

                SgClassDeclaration* defClassDecl = isSgClassDeclaration(currClassDecl->get_definingDeclaration());
                if(defClassDecl == NULL) { // Can get a NULL here if a primative type is being constructed. 
                  continue;                // For example, a pointer
                }
                SgClassDefinition* currClass = currClassDecl->get_definition();
                if(currClass == NULL) { // Can get a NULL here if class is an anonymous compiler generated BaseClass
                  continue;    
                }

                foreach(SgBaseClass* baseClass, currClass->get_inheritances())
                {
                    ROSE_ASSERT(baseClass->get_base_class() != NULL);
                    SgMemberFunctionDeclaration* constructorCalled = SageInterface::getDefaultConstructor(baseClass->get_base_class());

                    if (constructorCalled != NULL && !constructorCalled->get_file_info()->isCompilerGenerated())
                    {
                        props.push_back(constructorCalled);
                    }
                    else
                    {
                        worklist.push_back(baseClass->get_base_class());
                    }
                }
            }
        }
    }
    else
    {
        SgMemberFunctionDeclaration* decl = isSgMemberFunctionDeclaration(memFunDecl->get_firstNondefiningDeclaration());
        if (decl == NULL)
            decl = isSgMemberFunctionDeclaration(memFunDecl->get_definingDeclaration());
        ROSE_ASSERT(decl != NULL);
        props.push_back(decl);
    }

    return props;
}

//
// Add the declaration for functionCallExp to functionList. In the case of
// function pointers and virtual functions, append the set of declarations
// to functionList.
void
getPropertiesForSgConstructorInitializer(SgConstructorInitializer* sgCtorInit,
                         ClassHierarchyWrapper* classHierarchy,
                         Rose_STL_Container<SgFunctionDeclaration *>& functionList)
{
  // currently, all constructor initializers can be handled by solveConstructorInitializer
  const std::vector<SgFunctionDeclaration*>& props = CallTargetSet::solveConstructorInitializer(sgCtorInit);
  functionList.insert(functionList.end(), props.begin(), props.end());
}

/** Add the declaration for functionCallExp to functionList. In the case of function pointers and virtual functions, append the
 *  set of declarations to functionList. */
void
getPropertiesForSgFunctionCallExp(SgFunctionCallExp* sgFunCallExp,
                                  ClassHierarchyWrapper* classHierarchy,
                                  Rose_STL_Container<SgFunctionDeclaration*>& functionList,
                                  bool includePureVirtualFunc = false)
{
    SgExpression* functionExp = sgFunCallExp->get_function();
    ROSE_ASSERT(functionExp != NULL);

    while (isSgCommaOpExp(functionExp))
        functionExp = isSgCommaOpExp(functionExp)->get_rhs_operand();

    switch (functionExp->variantT()) {
        case V_SgArrowStarOp:
        case V_SgDotStarOp: {
            std::vector<SgFunctionDeclaration*> fD = CallTargetSet::solveMemberFunctionPointerCall(functionExp, classHierarchy);
            functionList.insert(functionList.end(), fD.begin(), fD.end());
            break;
        }

        case V_SgDotExp:
        case V_SgArrowExp: {
            ROSE_ASSERT(isSgBinaryOp(functionExp));

            SgExpression* leftSide = isSgBinaryOp(functionExp)->get_lhs_operand();
            SgType* const receiverType = leftSide->get_type();
            SgType* const leftType = receiverType->findBaseType();
            SgClassType*  crtClass = isSgClassType(leftType);

            if (SgMemberFunctionRefExp * memberFunctionRefExp =
                isSgMemberFunctionRefExp(isSgBinaryOp(functionExp)->get_rhs_operand())) {
                // AS(122805) In the case of a constructor initializer it is possible that a call to a constructor initializer
                // may return a type corresponding to an operator some-type() declared within the constructed class. An example
                // is:
                //   struct Foo {
                //      operator  bool () const
                //          { return true; }
                //   };
                //
                //   struct Bar {
                //      bool foobar()
                //          { return Foo (); }
                //   };
                // where the call to the constructor of the class Foo will cause a call to the operator bool(), where bool
                // corresponds type of the member function foobar declared within Bar.
                if (isSgConstructorInitializer(leftSide)) {
                    SgClassDeclaration* constInit = isSgConstructorInitializer(leftSide)->get_class_decl();
                    if (constInit) {
                        crtClass = constInit->get_type();
                    } else {
                        // AS(010306) A compiler constructed SgConstructorInitializer may wrap a function call which return a
                        // class type. In an dot or arrow expression this returned class type may be used as an expression left
                        // hand side. To handle this case the returned class type must be extracted from the expression
                        // list. An example demonstrating this is:
                        //class Vector3d {
                        //   public:
                        //    Vector3d(){};
                        //    Vector3d(const Vector3d &vector3d){};
                        //   Vector3d     cross() const
                        //        { return Vector3d();};
                        //   void   GetZ(){};
                        //};
                        //void foo(){
                        //  Vector3d vn1;
                        //  (vn1.cross()).GetZ();
                        //}
                        SgExprListExp* expLst = isSgExprListExp(isSgConstructorInitializer(leftSide)->get_args());
                        ROSE_ASSERT(expLst != NULL);
                        ROSE_ASSERT(expLst->get_expressions().size() == 1);
                        SgClassType* lhsClassType = isSgClassType(isSgFunctionCallExp(*expLst->get_expressions().begin())
                                                                  ->get_type()->stripType(SgType::STRIP_TYPEDEF_TYPE));
                        crtClass = lhsClassType;
                    }
                    ROSE_ASSERT(crtClass != NULL);
                }

                if (crtClass == NULL) {
               // TV (10/26/2018) : I have leftType = SgFunctionType which make sense: when was buildCallGraph tested last!!
                  break; // FIXME ROSE-1487
                }

                SgMemberFunctionDeclaration* memberFunctionDeclaration =
                    isSgMemberFunctionDeclaration(memberFunctionRefExp->get_symbol()->get_declaration());
                ROSE_ASSERT(memberFunctionDeclaration && crtClass);

                //Set function to first non-defining declaration
                SgMemberFunctionDeclaration *nonDefDecl =
                    isSgMemberFunctionDeclaration(memberFunctionDeclaration->get_firstNondefiningDeclaration());
                if (nonDefDecl)
                    memberFunctionDeclaration = nonDefDecl;

#if 0
                // PP (01/28/20): virtual function calls
                //   * all calls to virtual functions are polymorphic.
                //       e.g., x() or this->x() are both virtual (assuming x() is a member function call)
                //   * exceptions include:
                //     - suppressed virtual calls: A::x() always calls x in A
                //     - calls from a constructor or destructor (also indirectly
                //       by other functions while ctor/dtor are active) are non-virtual.
                //       (HANDLING REQUIRES CALLGRAPH INFO).
                //   * in addition:
                //     - if the receiver object is not a pointer, reference, or rvalue-reference
                //       the call can be statically dispatched.

                // was:
                // returns the list of all in-class declarations of functions potentially called
                // ( may be several because of polymorphism )
                //~ bool polymorphic = false;
                //~ if (!isSgThisExp(leftSide))
                    //~ polymorphic = true;
#endif
                // test if the memberFunctionRefExp is scope qualified
                //   in which case the vcall is suppressed.
                // \note to handle constructors correctly, we would need the assumed call stack.


                SgType* const receiverBaseType = receiverType->stripTypedefsAndModifiers();
                const bool    polymorphicType  = (  isSgPointerType(receiverBaseType)
                                                 || isSgReferenceType(receiverBaseType)
                                                 || isSgRvalueReferenceType(receiverBaseType)
                                                 || isSgArrayType(receiverBaseType)
                                                 );

                const bool    polymorphicCall  = (  polymorphicType
                                                 && (memberFunctionRefExp->get_need_qualifier() == 0)
                                                 );

                std::vector<SgFunctionDeclaration*> fD =
                    CallTargetSet::solveMemberFunctionCall(crtClass, classHierarchy, memberFunctionDeclaration, polymorphicCall,
                                                           includePureVirtualFunc);
                functionList.insert(functionList.end(), fD.begin(), fD.end());
            }
            break;
        }

        case V_SgPointerDerefExp: {
            SgPointerDerefExp *exp = isSgPointerDerefExp(functionExp);
            // If the thing pointed to is ultimately a SgFunctionRefExp then we can figure out the exact function that's being
            // pointed to just by following the pointers to the SgFunctionRefExp.  EDG3 never generated this kind of AST
            // because it removed the extraneous SgPointerDerefExp nodes.  I.e., for input like this:
            //   void g() { (********g)(); }
            // EDG3's AST would not have any SgFunctionRefExp nodes, but EDG4 leaves all of them there.
            // [Robb Matzke 2012-12-28]
            SgFunctionRefExp *fref = NULL;
            while (exp && !fref) {
                fref = isSgFunctionRefExp(exp->get_operand_i());
                exp = isSgPointerDerefExp(exp->get_operand_i());
            }
            if (!fref) {
                // We don't know what function is being called, only its type.  So assume that all functions whose type matches
                // could be called. [Robb Matzke 2012-12-28]
                std::vector<SgFunctionDeclaration*> fD =
                    CallTargetSet::solveFunctionPointerCall(isSgPointerDerefExp(functionExp), SageInterface::getProject());
                functionList.insert(functionList.end(), fD.begin(), fD.end());
                break;
            } else {
                // We know the function being called, so fall through to the SgFunctionRefExp case.
                functionExp = fref;
            }
        }
        // fall through...

        case V_SgMemberFunctionRefExp:
        case V_SgFunctionRefExp: {
            SgFunctionDeclaration *fctDecl =
                isSgFunctionRefExp(functionExp) ?
                isSgFunctionDeclaration(isSgFunctionRefExp(functionExp)->get_symbol()->get_declaration()) :
                isSgFunctionDeclaration(isSgMemberFunctionRefExp(functionExp)->get_symbol()->get_declaration());
            ROSE_ASSERT(fctDecl);
            assert(!isSgTemplateFunctionDeclaration(fctDecl));
            SgFunctionDeclaration *nonDefDecl = isSgFunctionDeclaration(fctDecl->get_firstNondefiningDeclaration());

            //Construction Function Props
            if (nonDefDecl) {
                assert(!isSgTemplateFunctionDeclaration(nonDefDecl));
                fctDecl = nonDefDecl;
            }

            functionList.push_back(fctDecl);
            break;
        }

        case V_SgVarRefExp: {
            // This is an indirect function call, as in:
            //    |void f() {
            //    |    void (*g)();
            //    |    g();              <------------
            //    |}
            // We don't know what is being called, only its type.  So assume that all functions whose type matches could be
            // called. [Robb P. Matzke 2013-01-24]
            VariantVector vv;
            vv.push_back(V_SgFunctionDeclaration);
            vv.push_back(V_SgTemplateInstantiationFunctionDecl);
            SgType *type = isSgVarRefExp(functionExp)->get_type();
            while (isSgTypedefType(type))
                type = isSgTypedefType(type)->get_base_type();
            SgPointerType *functionPointerType = isSgPointerType(type);

            if (functionPointerType == NULL) break; // FIXME ROSE-1487

            assert(functionPointerType!=NULL);
            SgFunctionType *fctType = isSgFunctionType(functionPointerType->findBaseType());
            assert(fctType!=NULL);
            SgFunctionDeclarationPtrList matches =
                AstQueryNamespace::queryMemoryPool(std::bind2nd(std::ptr_fun(solveFunctionPointerCallsFunctional), fctType),
                                                   &vv);
            functionList.insert(functionList.end(), matches.begin(), matches.end());
            break;
        }

        case V_SgPntrArrRefExp:
        case V_SgCastExp:
             break; // FIXME ROSE-1487
             
        // \todo 
        // PP (04/06/20) 
        case V_SgTemplateFunctionRefExp:
        case V_SgNonrealRefExp:
        case V_SgTemplateMemberFunctionRefExp:
        case V_SgConstructorInitializer:
        case V_SgFunctionCallExp:
             break;  

        default: {
            cout << "Error, unexpected type of functionRefExp: " << functionExp->sage_class_name() << "!!!\n";
            ROSE_ASSERT(false);
        }
    }
}
// Add the declaration for functionCallExp to functionList. In the case of
// function pointers and virtual functions, append the set of declarations
// to functionList.

void
CallTargetSet::getPropertiesForExpression(SgExpression* sgexp, ClassHierarchyWrapper* classHierarchy,
        Rose_STL_Container<SgFunctionDeclaration*>& functionList, bool includePureVirtualFunc)
{
    switch (sgexp->variantT())
    {
        case V_SgFunctionCallExp:
        {
            getPropertiesForSgFunctionCallExp(isSgFunctionCallExp(sgexp), classHierarchy, functionList, includePureVirtualFunc);
            break;
        }
        case V_SgConstructorInitializer:
        {
            getPropertiesForSgConstructorInitializer(isSgConstructorInitializer(sgexp), classHierarchy, functionList);
            break;
        }
        default:
        {
            std::cerr << "Error: cannot determine Properties for " << sgexp->class_name() << std::endl;
            break;
        }
    }
}

void CallTargetSet::getDeclarationsForExpression(SgExpression* exp,
                ClassHierarchyWrapper* classHierarchy,
                Rose_STL_Container<SgFunctionDeclaration*>& defList,
                bool includePureVirtualFunc)
{
        Rose_STL_Container<SgFunctionDeclaration*> props;
        CallTargetSet::getPropertiesForExpression(exp, classHierarchy, props, includePureVirtualFunc);

        foreach(SgFunctionDeclaration* candidateDecl, props)
        {
                ROSE_ASSERT(candidateDecl);
                assert(!isSgTemplateFunctionDeclaration(candidateDecl));
                defList.push_back(candidateDecl);
        }
}

void
CallTargetSet::getDefinitionsForExpression(SgExpression* sgexp,
        ClassHierarchyWrapper* classHierarchy,
        Rose_STL_Container<SgFunctionDefinition*>& defList)
{
    Rose_STL_Container<SgFunctionDeclaration*> props;
    CallTargetSet::getPropertiesForExpression(sgexp, classHierarchy, props);

    foreach(SgFunctionDeclaration* candidateDecl, props)
    {
        ROSE_ASSERT(candidateDecl);
        assert(!isSgTemplateFunctionDeclaration(candidateDecl));
        candidateDecl = isSgFunctionDeclaration(candidateDecl->get_definingDeclaration());
        if (candidateDecl != NULL)
        {
            SgFunctionDefinition* candidateDef = candidateDecl->get_definition();
            if (candidateDef != NULL)
            {
                defList.push_back(candidateDef);
            }
        }
    }
}

void
CallTargetSet::getExpressionsForDefinition(SgFunctionDefinition* targetDef,
                                           ClassHierarchyWrapper* classHierarchy,
                                           Rose_STL_Container<SgExpression*>& exps) {
  VariantVector vv(V_SgFunctionCallExp);
  Rose_STL_Container<SgNode*> callCandidates = NodeQuery::queryMemoryPool(vv);
  foreach (SgNode* callCandidate, callCandidates) {
    SgFunctionCallExp* callexp = isSgFunctionCallExp(callCandidate);
    Rose_STL_Container<SgFunctionDefinition*> candidateDefs;
    CallTargetSet::getDefinitionsForExpression(callexp, classHierarchy, candidateDefs);
    foreach (SgFunctionDefinition* candidateDef, candidateDefs) {
      if (candidateDef == targetDef) {
        exps.push_back(callexp);
        break;
      }
    }
  }
  VariantVector vv2(V_SgConstructorInitializer);
  Rose_STL_Container<SgNode*> ctorCandidates = NodeQuery::queryMemoryPool(vv2);
  foreach (SgNode* ctorCandidate, ctorCandidates) {
    SgConstructorInitializer* ctorInit = isSgConstructorInitializer(ctorCandidate);
    Rose_STL_Container<SgFunctionDefinition*> candidateDefs;
    CallTargetSet::getDefinitionsForExpression(ctorInit, classHierarchy, candidateDefs);
    foreach (SgFunctionDefinition* candidateDef, candidateDefs) {
      if (candidateDef == targetDef) {
        exps.push_back(ctorInit);
        break;
      }
    }
  }
}

FunctionData::FunctionData ( SgFunctionDeclaration* inputFunctionDeclaration,
    SgProject *project, ClassHierarchyWrapper *classHierarchy )
{
    hasDefinition = false;

    functionDeclaration = inputFunctionDeclaration;
    assert(!isSgTemplateFunctionDeclaration(functionDeclaration));

    SgFunctionDeclaration *defDecl =
            (
            inputFunctionDeclaration->get_definition() != NULL ?
            inputFunctionDeclaration : isSgFunctionDeclaration(functionDeclaration->get_definingDeclaration())
            );

    if (defDecl != NULL && defDecl->get_definition() == NULL)
    {
        defDecl = NULL;
        std::cerr << " **** If you see this error message. Report to the ROSE team that a function declaration ****\n"
                << " **** has a defining declaration but no definition                                       ****\n";
    }

    //cout << "!!!" << inputFunctionDeclaration->get_name().str() << " has definition " << defDecl << "\n";
    //     cout << "Input declaration: " << inputFunctionDeclaration << " as opposed to " << functionDeclaration << "\n";

    // Test for a forward declaration (declaration without a definition)
    if (defDecl != NULL)
    {
        hasDefinition = true;

        Rose_STL_Container<SgNode*> functionCallExpList = NodeQuery::querySubTree(defDecl, V_SgFunctionCallExp);
        foreach(SgNode* functionCallExp, functionCallExpList)
        {
            CallTargetSet::getPropertiesForExpression(isSgExpression(functionCallExp), classHierarchy,  functionList);
        }

        Rose_STL_Container<SgNode*> ctorInitList = NodeQuery::querySubTree(defDecl, V_SgConstructorInitializer);
        foreach(SgNode* ctorInit, ctorInitList)
        {
            CallTargetSet::getPropertiesForExpression(isSgExpression(ctorInit), classHierarchy, functionList);
        }
    }
}
SgFunctionDeclaration * CallTargetSet::getFirstVirtualFunctionDefinitionFromAncestors(SgClassType *crtClass,
        SgMemberFunctionDeclaration *memberFunctionDeclaration, ClassHierarchyWrapper *classHierarchy)  {

    SgFunctionDeclaration *resultDecl = NULL;

    ROSE_ASSERT ( memberFunctionDeclaration && classHierarchy );
  //  memberFunctionDeclaration->get_file_info()->display( "Member function we are considering" );
    SgDeclarationStatement *nonDefDeclInClass = NULL;

  nonDefDeclInClass = memberFunctionDeclaration->get_firstNondefiningDeclaration();
  SgMemberFunctionDeclaration *functionDeclarationInClass = NULL;

  // memberFunctionDeclaration is outside the class
  if ( nonDefDeclInClass)   {
   functionDeclarationInClass = isSgMemberFunctionDeclaration( nonDefDeclInClass );
  }
  // in class declaration, since there is no non-defining declaration
  else {
    functionDeclarationInClass = memberFunctionDeclaration;
    //      functionDeclarationInClass->get_file_info()->display("declaration in class already");
  }

  ROSE_ASSERT ( functionDeclarationInClass );

   if(!(functionDeclarationInClass->get_functionModifier().isVirtual())) return NULL;
    // for virtual functions, we need to search down in the hierarchy of classes
    // and retrieve all declarations of member functions with the same type

  SgClassDefinition *crtClsDef = NULL;

  if ( crtClass )  {
      SgClassDeclaration *tmp = isSgClassDeclaration( crtClass->get_declaration() );
      ROSE_ASSERT ( tmp );
      SgClassDeclaration* tmp2 = isSgClassDeclaration(tmp->get_definingDeclaration());
      ROSE_ASSERT (tmp2);
      crtClsDef = tmp2->get_definition();
      ROSE_ASSERT ( crtClsDef );
  }  else  {
      crtClsDef = isSgClassDefinition( memberFunctionDeclaration->get_scope() );
      ROSE_ASSERT ( crtClsDef );
  }

    functionDeclarationInClass = NULL;
    vector<SgClassDefinition*> worklist;
    worklist.push_back(crtClsDef);

    while(!worklist.empty()) {
        SgClassDefinition* ancestor = worklist.back();
        worklist.pop_back();
        SgClassDefinition *cls = isSgClassDefinition( ancestor );
        resultDecl = is_function_exists(cls, memberFunctionDeclaration );
        if(resultDecl != NULL)
              return resultDecl;
        const ClassHierarchyWrapper::ClassDefSet& ancestors = classHierarchy->getAncestorClasses( cls );
        worklist.insert(worklist.end(), ancestors.begin(), ancestors.end());
    }

    return NULL;
}

void
CallGraphBuilder::buildCallGraph (){
  buildCallGraph(dummyFilter());
}


/**
 *  CallGraphBuilder::hasGraphNodeFor
 *
 * \brief Checks the graphNodes map for a graph node match fdecl
 *
 * This does a lookup on the CallGraph map to see if a given function is in it.
 * This is used in constructing the CallGraph.  If multiple files are input on 
 * the command line firstNondefiningDeclartion may not be unique, so not finding
 * a graphNode for fdecl does not guarantee that the target function does not exist
 * in the graph, only that it is not in the lookup map.  Use getGraphNodeFor 
 * to be sure.  Again, this is mainly used in constructing the call graph.
 *
 * \param[in] fdecl The declaration of the function to look for in the graph
 *
 **/
SgGraphNode * CallGraphBuilder::hasGraphNodeFor(SgFunctionDeclaration * fdecl) const {
  SgFunctionDeclaration *unique = isSgFunctionDeclaration(fdecl->get_firstNondefiningDeclaration());
  GraphNodes::const_iterator lookedup = graphNodes.find(unique); 
  if(lookedup != graphNodes.end()) {
    return lookedup->second;
  }
  return NULL;
}

/**
 *  CallGraphBuilder::getGraphNodeFor
 *
 * \brief Double checks that the Call Graph has the function
 *
 * This double checks for a call graph node.  If multiple files are input on the command
 * line firstNondefiningDeclartion may not be unique.  (As expected) So we can double check
 * on the name.  This is useful only outside the CallGraph, if the CallGraph is fully
 * constructed, and we need a particular node from it.
 *
 * \param[in] fdecl The declaration of the function to look for in the graph
 *
 **/
SgGraphNode * CallGraphBuilder::getGraphNodeFor(SgFunctionDeclaration * fdecl) const {
  SgFunctionDeclaration *unique = isSgFunctionDeclaration(fdecl->get_firstNondefiningDeclaration());
  GraphNodes::const_iterator lookedup = graphNodes.find(unique); 
  if(lookedup != graphNodes.end()) {
    return lookedup->second;
  }

  //Fall back on old slow method (When putting multiple 
  std::string fname = fdecl->get_mangled_name();
  for (GraphNodes::const_iterator it = graphNodes.begin(); it != graphNodes.end(); ++it )
    if (it->first->get_mangled_name() == fname)
      return it->second;
  return NULL;
}

  GetOneFuncDeclarationPerFunction::result_type
GetOneFuncDeclarationPerFunction::operator()(SgNode* node )
{
  result_type returnType;
  SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(node);
  if(funcDecl != NULL && !isSgTemplateFunctionDeclaration(funcDecl))
  {
    if( funcDecl->get_definingDeclaration () != NULL && node == funcDecl->get_definingDeclaration ())
      returnType.push_back(node);
    if( funcDecl->get_definingDeclaration () == NULL && node == funcDecl->get_firstNondefiningDeclaration () )
      returnType.push_back(node);
  }
  return returnType;
}

