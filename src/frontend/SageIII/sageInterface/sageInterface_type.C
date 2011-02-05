#include "sage3basic.h"
#include <sstream>
#include <iostream>
#include "sageInterface.h"
#include <map>
using namespace std;


// originally from src/midend/astInlining/typeTraits.C
// src/midend/astUtil/astInterface/AstInterface.C
namespace SageInterface
{
  //! Internal map between SgType and the corresponding type string specific to GASnet
  // support mangled type names according to Itanium C++ ABI
  map <SgType*,string> type_string_map; 
  map <string, SgType*> string_type_map; 

  //! Check if a type is an integral type, only allowing signed/unsigned short, int, long, long long.
  ////!
  ////! There is another similar function named SgType::isIntegerType(), which allows additional types char, wchar, and bool.
  bool isStrictIntegerType(SgType* t)
  {
    ROSE_ASSERT(t != NULL);

    while (t->variant() == T_TYPEDEF)
    {
      t = ((SgTypedefType *) t)->get_base_type();
      ROSE_ASSERT(t != NULL);
    }

    switch (t->variant())
    {
      case T_SHORT:
      case T_SIGNED_SHORT:
      case T_UNSIGNED_SHORT:
      case T_INT:
      case T_SIGNED_INT:
      case T_UNSIGNED_INT:
      case T_LONG:
      case T_SIGNED_LONG:
      case T_UNSIGNED_LONG:
      case T_LONG_LONG:
      case T_SIGNED_LONG_LONG:
      case T_UNSIGNED_LONG_LONG:
        return true; // 1
        break;
      default:
        return false; // 0
    }
  }

  bool isScalarType( SgType* t)
  {
    ROSE_ASSERT(t);
    switch(t->variantT()) {
      case V_SgTypedefType:
         return isScalarType(isSgTypedefType(t)->get_base_type());
      case V_SgTypeChar :
      case V_SgTypeSignedChar :
      case V_SgTypeUnsignedChar :

      case V_SgTypeShort :
      case V_SgTypeSignedShort :
      case V_SgTypeUnsignedShort :

      case V_SgTypeInt :
      case V_SgTypeSignedInt :
      case V_SgTypeUnsignedInt :

      case V_SgTypeLong :
      case V_SgTypeSignedLong :
      case V_SgTypeUnsignedLong :

      case V_SgTypeVoid :
      case V_SgTypeWchar:
      case V_SgTypeFloat:
      case V_SgTypeDouble:

      case V_SgTypeLongLong:
      case V_SgTypeUnsignedLongLong:
      case V_SgTypeLongDouble:

      case V_SgTypeString:
      case V_SgTypeBool:
        // DQ (8/27/2006): change name of SgComplex to SgTypeComplex (for consistency) and added SgTypeImaginary (C99 type).
      case V_SgTypeComplex:
      case V_SgTypeImaginary:
        return true;
      default: {}
    }
    return false;
  }


  bool isStructType(SgType* t)
  {
    bool result = false;
    ROSE_ASSERT(t!=NULL);
    SgClassType * class_type = isSgClassType(t);
    if (class_type)
    {
      SgDeclarationStatement* decl = class_type->get_declaration();
      result = isStructDeclaration(decl);
    }
    return result;
  }

  SgType* getBaseFromType(SgType* t) {

    SgType* deref = t->dereference();
    if (deref && deref != t)
      return getBaseFromType(deref);
    return t;
  }

  // Is this a const type?
  bool isConstType(SgType* t) {
    //  if (isSgReferenceType(t)) // reference type cannot be cv-qualified usually, 
    //    return isConstType(isSgReferenceType(t)->get_base_type());
    if (isSgTypedefType(t))
      return isConstType(isSgTypedefType(t)->get_base_type());
    if (isSgModifierType(t)) {
      SgTypeModifier& modifier = isSgModifierType(t)->get_typeModifier();
      SgConstVolatileModifier& cv = modifier.get_constVolatileModifier();
      if (cv.isConst())
        return true;
      else
        return isConstType(isSgModifierType(t)->get_base_type());
    }
    return false;
  }

  // Remove a top-level const from the type, if present
  SgType* removeConst(SgType* t) {
    if (isSgTypedefType(t))
      return removeConst(isSgTypedefType(t)->get_base_type());
    if (isSgModifierType(t)) {
      SgModifierType* newT = deepCopy(isSgModifierType(t));
      ROSE_ASSERT (newT);
      SgTypeModifier& modifier = newT->get_typeModifier();
      SgConstVolatileModifier& cv = modifier.get_constVolatileModifier();
      cv.unsetConst();
      return newT;
    }
    return t;
  }

  // Is this a volatile type?
  bool isVolatileType(SgType* t) {
    if (isSgTypedefType(t))
      return isVolatileType(isSgTypedefType(t)->get_base_type());
    if (isSgModifierType(t)) {
      SgTypeModifier& modifier = isSgModifierType(t)->get_typeModifier();
      SgConstVolatileModifier& cv = modifier.get_constVolatileModifier();
      if (cv.isVolatile())
        return true;
      else
        return isVolatileType(isSgModifierType(t)->get_base_type());
    }
    return false;
  }

  // Is this a restrict type?
  bool isRestrictType(SgType* t) {
    if (isSgTypedefType(t))
      return isRestrictType(isSgTypedefType(t)->get_base_type());
    if (isSgModifierType(t)) {
      SgTypeModifier& modifier = isSgModifierType(t)->get_typeModifier();
      if (modifier.isRestrict())
        return true;
      else
        return isRestrictType(isSgModifierType(t)->get_base_type());
    }
    return false;
  }


  // Is this type a non-constant reference type?
  bool isNonconstReference(SgType* t) {
    if (isSgReferenceType(t))
      return !isConstType(isSgReferenceType(t)->get_base_type());
    if (isSgTypedefType(t))
      return isNonconstReference(isSgTypedefType(t)->get_base_type());
    if (isSgModifierType(t)) {
      SgTypeModifier& modifier = isSgModifierType(t)->get_typeModifier();
      SgConstVolatileModifier& cv = modifier.get_constVolatileModifier();
      if (cv.isConst())
        return false;
      else
        return isNonconstReference(isSgModifierType(t)->get_base_type());
    }
    return false;
  }

  // Is this type a constant or non-constant reference type?
  bool isReferenceType(SgType* t) {
    if (isSgReferenceType(t))
      return true;
    if (isSgTypedefType(t))
      return isReferenceType(isSgTypedefType(t)->get_base_type());
    if (isSgModifierType(t)) {
      return isReferenceType(isSgModifierType(t)->get_base_type());
    }
    return false;
  }

  // Is this type a pointer type?
  bool isPointerType(SgType* t) {
    if (isSgPointerType(t))
      return true;
    if (isSgTypedefType(t))
      return isPointerType(isSgTypedefType(t)->get_base_type());
    if (isSgModifierType(t)) {
      return isPointerType(isSgModifierType(t)->get_base_type());
    }
    return false;
  }

bool isPointerToNonConstType(SgType* type)
{
	if (SgTypedefType* typeDef = isSgTypedefType(type))
		return isPointerToNonConstType(typeDef->get_base_type());
	else if (SgPointerType* pointerType = isSgPointerType(type))
		return !SageInterface::isConstType(pointerType->get_base_type());
	else if (SgModifierType* modifierType = isSgModifierType(type))
		return isPointerToNonConstType(modifierType->get_base_type());
	else
		return false;
}

  //! Check if an expression is an array access. If so, return its name and subscripts if requested. Based on AstInterface::IsArrayAccess()
  bool isArrayReference(SgExpression* ref, SgExpression** arrayName/*=NULL*/, vector<SgExpression*>** subscripts/*=NULL*/)
  {
    SgExpression* arrayRef=NULL;
    if (ref->variantT() == V_SgPntrArrRefExp) {
      if (subscripts != 0 || arrayName != 0) {
        SgExpression* n = ref;
        while (true) {
          SgPntrArrRefExp *arr = isSgPntrArrRefExp(n);
          if (arr == 0)
            break;
          n = arr->get_lhs_operand();
          // store left hand for possible reference exp to array variable
          if (arrayName!= 0)
            arrayRef = n;
          // right hand stores subscripts
          if (subscripts != 0)
            (*subscripts)->push_back(arr->get_rhs_operand());
        } // end while
        if  (arrayName !=NULL)
        {
          *arrayName = arrayRef;
#if 0 // we decided to delegate this to convertRefToInitializedName()
          ROSE_ASSERT(arrayRef != NULL);
          if (isSgVarRefExp(arrayRef))
            *arrayName = isSgVarRefExp(arrayRef)->get_symbol()->get_declaration();
          else if (isSgDotExp(arrayRef))
          { // just return the parent object name for now 
             // this may not be a good choice,but we do this for collect referenced top variables
              // within outliner  
            SgVarRefExp* lhs = NULL;
            lhs = isSgVarRefExp(isSgDotExp(arrayRef)->get_lhs_operand());
            ROSE_ASSERT(lhs != NULL);
            *arrayName = lhs->get_symbol()->get_declaration();
          }
          else
           {
            cout<<"Unhandled case in isArrayReference():"<<arrayRef->class_name()<<" "
            arrayRef->unparseToString()<<endl;
            ROSE_ASSERT(false);
           }
#endif 
        }
      }
      return true;
    }
    return false;
  }
 
 
  //! Calculate the number of elements of an array type
  size_t getArrayElementCount(SgArrayType* t)
  {
    ROSE_ASSERT(t);
    size_t result=1; 
    SgExpression * indexExp =  t->get_index();

    //strip off THREADS for UPC array with a dimension like dim*THREADS
    if (isUpcArrayWithThreads(t))
    {
      SgMultiplyOp* multiply = isSgMultiplyOp(indexExp);
      ROSE_ASSERT(multiply);
      indexExp = multiply->get_lhs_operand();
    }

    // assume dimension default to 1 if not specified ,such as a[] 
    if (indexExp == NULL) 
      result = 1;
    else 
    { 
      //Take advantage of the fact that the value expression is always SgUnsignedLongVal in AST
      SgUnsignedLongVal * valExp = isSgUnsignedLongVal(indexExp);
      SgIntVal * valExpInt = isSgIntVal(indexExp);
      ROSE_ASSERT(valExp || valExpInt); // TODO: return -1 is better ?
      if (valExp)
        result = valExp->get_value(); 
      else 
        result = valExpInt->get_value(); 
    }

    // consider multi dimensional case 
    SgArrayType* arraybase = isSgArrayType(t->get_base_type());
    if (arraybase)
      result = result * getArrayElementCount(arraybase);

    return result;
  } // getArrayElementCount()

  SgType* getArrayElementType(SgType* t)
  {
    ROSE_ASSERT(t);
    SgType* current_type = t->stripType(SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_TYPEDEF_TYPE | SgType::STRIP_REFERENCE_TYPE);
    while (isSgArrayType(current_type))
      current_type = isSgArrayType(current_type)->get_base_type()->stripTypedefsAndModifiers();
    if (current_type->variantT() == V_SgTypeString)
      return SgTypeChar::createType();
    return current_type;  
  }

  SgType* getElementType(SgType* t)
  {
    ROSE_ASSERT(t);
    t = t->stripType(SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_TYPEDEF_TYPE | SgType::STRIP_REFERENCE_TYPE);
    VariantT vt = t->variantT();
    if (vt == V_SgPointerType)
      return static_cast<SgPointerType *>(t)->get_base_type();
    else if (vt == V_SgArrayType)
      return static_cast<SgArrayType *>(t)->get_base_type();
    else if (vt == V_SgTypeString)
      return SgTypeChar::createType();
    else
      return NULL;
  }

  //! Get the number of dimensions of an array type
  int getDimensionCount(SgType* mytype)
  {
    ROSE_ASSERT(mytype != NULL);
    int dimension = 0;
    SgType* current_type = mytype->stripType(SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_TYPEDEF_TYPE | SgType::STRIP_REFERENCE_TYPE);
    while (isSgArrayType(current_type))
    {
      dimension++;
      current_type= isSgArrayType(current_type)->get_base_type()->stripTypedefsAndModifiers();
    }
    if (current_type->variantT() == V_SgTypeString)
      dimension++;
    return dimension;
  }                              
  // Returns true if args is a valid argument type list for decl
  // Requires the exact argument list given -- conversions not handled
  // Good enough for default and copy constructors, but not other functions
  static bool acceptsArguments(SgFunctionDeclaration* decl, 
      std::list<SgType*> args) {
    SgInitializedNamePtrList& params = decl->get_args();
    SgInitializedNamePtrList::iterator p = params.begin();
    std::list<SgType*>::iterator a = args.begin();
    for (; p != params.end() && a != args.end(); ++p, ++a) {
      if (getBaseFromType((*p)->get_type()) != getBaseFromType(*a))
        return false; // Type mismatch
      // Good enough for copy constructors, though
    }
    if (p == params.end() && a != args.end())
      return false; // Too many arguments
    for (; p != params.end(); ++p) {
      if ((*p)->get_initializer() == 0)
        return false; // Non-defaulted parameter after end of arg list
    }
    return true;
  }

  // Is a type default constructible?  This may not quite work properly.
  bool isDefaultConstructible(SgType* type) {
    switch (type->variantT()) {
      case V_SgArrayType:
        return isDefaultConstructible(isSgArrayType(type)->get_base_type());
        break;

      case V_SgModifierType:
        return isDefaultConstructible(isSgModifierType(type)->get_base_type()); 
        break;

      case V_SgClassType: {
        SgClassDeclaration* decl = 
          isSgClassDeclaration(isSgClassType(type)->get_declaration());
        assert (decl);
        SgClassDefinition* defn = decl->get_definition();
        assert (defn);
        bool hasDefaultConstructor = false, hasAnyConstructor = false;
        // Look for user-defined constructors
        SgDeclarationStatementPtrList& classMembers = defn->get_members();
        for (SgDeclarationStatementPtrList::iterator i = classMembers.begin();
            i != classMembers.end(); ++i) {
          if (isSgFunctionDeclaration(*i)) {
            SgFunctionDeclaration* fundecl = isSgFunctionDeclaration(*i);
            bool thisIsAConstructor = 
              fundecl->get_specialFunctionModifier().isConstructor();
            if (thisIsAConstructor)
              hasAnyConstructor = true;
            bool allArgsAreDefaulted = 
              acceptsArguments(fundecl, std::list<SgType*>());
            if (thisIsAConstructor && allArgsAreDefaulted &&
                (*i)->get_declarationModifier().get_accessModifier().isPublic())
              hasDefaultConstructor = true;
          }
        }
        if (hasDefaultConstructor) return true;
        if (hasAnyConstructor) return false;
        // Return true if all non-static data members are default constructible
        for (SgDeclarationStatementPtrList::iterator i = classMembers.begin();
            i != classMembers.end(); ++i) {
          if (isSgVariableDeclaration(*i)) {
            SgVariableDeclaration* decl = isSgVariableDeclaration(*i);
            if (decl->get_declarationModifier().get_storageModifier().isStatic())
              continue;
            SgInitializedNamePtrList& vars = decl->get_variables();
            for (SgInitializedNamePtrList::iterator j = vars.begin();
                j != vars.end(); ++j) {
              if (isDefaultConstructible((*j)->get_type()))
                return true;
            }
          }
        }
        return false;
      }
        break;

      case V_SgTypedefType:
        return isDefaultConstructible(isSgTypedefType(type)->get_base_type());
        break;

      case V_SgReferenceType:
        return false;
        break;

        // DQ (8/27/2006): Changed name of SgComplex to make it more consistant with other 
        // type names and added SgTypeImaginary IR node (for C99 complex support).
      case V_SgTypeComplex:
      case V_SgTypeImaginary:
      case V_SgEnumType:
      case V_SgPointerType:
      case V_SgPointerMemberType:
      case V_SgTypeBool:
      case V_SgTypeChar:
      case V_SgTypeDefault:
      case V_SgTypeDouble:
      case V_SgTypeFloat:
      case V_SgTypeGlobalVoid:
      case V_SgTypeInt:
      case V_SgTypeLong:
      case V_SgTypeLongDouble:
      case V_SgTypeLongLong:
      case V_SgTypeShort:
      case V_SgTypeSignedChar:
      case V_SgTypeSignedInt:
      case V_SgTypeSignedLong:
      case V_SgTypeSignedShort:
      case V_SgTypeString:
      case V_SgTypeUnsignedChar:
      case V_SgTypeUnsignedInt:
      case V_SgTypeUnsignedLong:
      case V_SgTypeUnsignedLongLong:
      case V_SgTypeUnsignedShort:
      case V_SgTypeVoid:
      case V_SgTypeWchar:
        return true;
        break;

      default:
        assert (!"Unknown type in isDefaultConstructible()");
        return true;
    }
  }

  // Is a type copy constructible?  This may not quite work properly.
  bool isCopyConstructible(SgType* type) {
    switch (type->variantT()) {
      case V_SgArrayType:
        return false; //Liao, array types are not copy constructible, 3/3/2009
        //return isCopyConstructible(isSgArrayType(type)->get_base_type());
        break;

      case V_SgModifierType:
        return isCopyConstructible(isSgModifierType(type)->get_base_type()); 
        break;

      case V_SgFunctionType:
      case V_SgMemberFunctionType:
      case V_SgTypeEllipse:
        return false;
        break;

      case V_SgClassType: {
        SgClassDeclaration* decl = 
          isSgClassDeclaration(isSgClassType(type)->get_declaration());
        assert (decl);
        assert (decl->get_definingDeclaration());
        // from forward declaration to defining declaration 
        SgClassDefinition* defn = isSgClassDeclaration(decl->get_definingDeclaration())->get_definition();
        assert (defn);
        bool hasPublicCopyConstructor = false, hasAnyCopyConstructor = false;
        // Look for user-defined constructors
        SgDeclarationStatementPtrList& classMembers = defn->get_members();
        for (SgDeclarationStatementPtrList::iterator i = classMembers.begin();
            i != classMembers.end(); ++i) {
          if (isSgFunctionDeclaration(*i)) {
            SgFunctionDeclaration* fundecl = isSgFunctionDeclaration(*i);
            bool thisIsAConstructor = 
              fundecl->get_specialFunctionModifier().isConstructor();
            std::list<SgType*> args;
            args.push_back(new SgReferenceType(type));
            bool isCopyConstructor = acceptsArguments(fundecl, args);
            if (thisIsAConstructor && isCopyConstructor) {
              hasAnyCopyConstructor = true;
              if ((*i)->get_declarationModifier().
                  get_accessModifier().isPublic())
                hasPublicCopyConstructor = true;
            }
          }
        }
        if (hasPublicCopyConstructor) return true;
        if (hasAnyCopyConstructor) return false;
        // Return true if all non-static data members are copy constructible
        for (SgDeclarationStatementPtrList::iterator i = classMembers.begin();
            i != classMembers.end(); ++i) {
          if (isSgVariableDeclaration(*i)) {
            SgVariableDeclaration* decl = isSgVariableDeclaration(*i);
            if (decl->get_declarationModifier().get_storageModifier().isStatic())
              continue;
            SgInitializedNamePtrList& vars = decl->get_variables();
            for (SgInitializedNamePtrList::iterator j = vars.begin();
                j != vars.end(); ++j) {
              if (!isCopyConstructible((*j)->get_type()))
                return false;
            }
          }
        }
        return true; // If no non-copy constructible data member, return true.
      }
        break;

      case V_SgTypedefType:
        return isCopyConstructible(isSgTypedefType(type)->get_base_type());
        break;

      case V_SgReferenceType:
        // DQ (8/27/2006): Changed name of SgComplex to make it more consistant with other 
        // type names and added SgTypeImaginary IR node (for C99 complex support).
      case V_SgTypeComplex:
      case V_SgTypeImaginary:
      case V_SgEnumType:
      case V_SgPointerType:
      case V_SgPointerMemberType:
      case V_SgTypeBool:
      case V_SgTypeChar:
      case V_SgTypeDefault:
      case V_SgTypeDouble:
      case V_SgTypeFloat:
      case V_SgTypeGlobalVoid:
      case V_SgTypeInt:
      case V_SgTypeLong:
      case V_SgTypeLongDouble:
      case V_SgTypeLongLong:
      case V_SgTypeShort:
      case V_SgTypeSignedChar:
      case V_SgTypeSignedInt:
      case V_SgTypeSignedLong:
      case V_SgTypeSignedShort:
      case V_SgTypeString:
      case V_SgTypeUnsignedChar:
      case V_SgTypeUnsignedInt:
      case V_SgTypeUnsignedLong:
      case V_SgTypeUnsignedLongLong:
      case V_SgTypeUnsignedShort:
      case V_SgTypeVoid:
      case V_SgTypeWchar:
        return true;
        break;

      default:
        std::cerr << "In SageInterface::isCopyConstructible(), type is a " << type->class_name() << std::endl;
        assert (!"Unknown type in isCopyConstructible()");
    }
    return true;
  }
  // Is a type assignable?  This may not quite work properly.
  // Liao, 3/3/2009 based on the code for isCopyConstructible()
  // TYPE a, b; is a=b; allowed ?
  bool isAssignable(SgType* type) {
    switch (type->variantT()) {
      case V_SgArrayType:
        return false; 
        break;

      case V_SgModifierType:
        return isAssignable(isSgModifierType(type)->get_base_type()); 
        break;

      case V_SgFunctionType:
      case V_SgMemberFunctionType:
      case V_SgReferenceType: //I think C++ reference types cannot be reassigned. 
      case V_SgTypeEllipse:
        return false;
        break;

      case V_SgClassType: {
        SgClassDeclaration* decl = isSgClassDeclaration(isSgClassType(type)->get_declaration());
        assert (decl);
        assert (decl->get_definingDeclaration());
        // from forward declaration to defining declaration 
        SgClassDefinition* defn = isSgClassDeclaration(decl->get_definingDeclaration())->get_definition();
        assert (defn);

        bool hasPublicAssignOperator= false, hasAnyAssignOperator = false;
        // Look for user-defined operator=
        SgDeclarationStatementPtrList& classMembers = defn->get_members();
        for (SgDeclarationStatementPtrList::iterator i = classMembers.begin();
            i != classMembers.end(); ++i) {
          if (isSgFunctionDeclaration(*i)) {
            SgFunctionDeclaration* fundecl = isSgFunctionDeclaration(*i);
            bool thisIsAnAssignOperator= 
              (fundecl->get_specialFunctionModifier().isConstructor())&&
              (fundecl->get_name().getString()=="operator=");
            // check parameter list has TYPE&      
            std::list<SgType*> args;
            args.push_back(new SgReferenceType(type));
            bool isAssignOperator= acceptsArguments(fundecl, args);
            if (thisIsAnAssignOperator&& isAssignOperator) {
              hasAnyAssignOperator= true;
              if ((*i)->get_declarationModifier().
                  get_accessModifier().isPublic())
                hasPublicAssignOperator= true;
            }
          }
        }
        if (hasPublicAssignOperator) return true;
        if (hasAnyAssignOperator) return false;
        // Return true if all non-static data members are assignable
        for (SgDeclarationStatementPtrList::iterator i = classMembers.begin();
            i != classMembers.end(); ++i) {
          if (isSgVariableDeclaration(*i)) {
            SgVariableDeclaration* decl = isSgVariableDeclaration(*i);
            if (decl->get_declarationModifier().get_storageModifier().isStatic())
              continue;
            SgInitializedNamePtrList& vars = decl->get_variables();
            for (SgInitializedNamePtrList::iterator j = vars.begin();
                j != vars.end(); ++j) {
              if (!isAssignable((*j)->get_type()))
                return false;
            }
          }
        }
        return true; // If no non-assignable data member, return true.
      }
        break;

      case V_SgTypedefType:
        return isAssignable(isSgTypedefType(type)->get_base_type());
        break;

      case V_SgTypeComplex: // C99 complex is assignable
      case V_SgTypeImaginary:
      case V_SgEnumType:
      case V_SgPointerType:
      case V_SgPointerMemberType:
      case V_SgTypeBool:
      case V_SgTypeChar:
      case V_SgTypeDefault:
      case V_SgTypeDouble:
      case V_SgTypeFloat:
      case V_SgTypeGlobalVoid:
      case V_SgTypeInt:
      case V_SgTypeLong:
      case V_SgTypeLongDouble:
      case V_SgTypeLongLong:
      case V_SgTypeShort:
      case V_SgTypeSignedChar:
      case V_SgTypeSignedInt:
      case V_SgTypeSignedLong:
      case V_SgTypeSignedShort:
      case V_SgTypeString:
      case V_SgTypeUnsignedChar:
      case V_SgTypeUnsignedInt:
      case V_SgTypeUnsignedLong:
      case V_SgTypeUnsignedLongLong:
      case V_SgTypeUnsignedShort:
      case V_SgTypeVoid:
      case V_SgTypeWchar:
        return true;
        break;

      default:
        std::cerr << "In SageInterface::isAssignable(), type is a " << type->class_name() << std::endl;
        assert (!"Unknown type in isAssignable()");
    }
    return true;
  }

  // Does a type have a trivial (built-in) destructor?
  bool hasTrivialDestructor(SgType* t) {
    switch (t->variantT()) {
      // DQ (8/27/2006): Changed name of SgComplex to make it more consistant with other 
      // type names and added SgTypeImaginary IR node (for C99 complex support).
      case V_SgTypeComplex:
      case V_SgTypeImaginary:
      case V_SgPointerType:
      case V_SgReferenceType:
      case V_SgTypeBool:
      case V_SgTypeChar:
      case V_SgTypeDouble:
      case V_SgTypeEllipse:
      case V_SgTypeFloat:
      case V_SgTypeGlobalVoid:
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
      case V_SgTypeUnsignedLongLong:
      case V_SgTypeUnsignedShort:
      case V_SgTypeVoid:
      case V_SgTypeWchar:
      case V_SgFunctionType:
      case V_SgTypeString:
      case V_SgEnumType:
        return true;

      case V_SgArrayType:
        return hasTrivialDestructor(isSgArrayType(t)->get_base_type());

      case V_SgModifierType: 
        return hasTrivialDestructor(isSgModifierType(t)->get_base_type());

      case V_SgTypedefType:
        return hasTrivialDestructor(isSgTypedefType(t)->get_base_type());

      default: 
        SgClassType* ct = isSgClassType(t);
        if (!ct) return false;
        SgDeclarationStatement* decl = ct->get_declaration();
        if (isSgClassDeclaration(decl)) {
          SgClassDeclaration* cd = isSgClassDeclaration(decl);
          SgClassDefinition* cdef = cd->get_definition();
          if (!cdef) return false;
          bool hasDestructor = false;
          SgDeclarationStatementPtrList& decls = cdef->get_members();
          for (SgDeclarationStatementPtrList::iterator i = decls.begin();
              i != decls.end(); ++i) {
            if (isSgFunctionDeclaration(*i)) {
              SgFunctionDeclaration* fundecl = isSgFunctionDeclaration(*i);
              if (fundecl->get_specialFunctionModifier().isDestructor())
                hasDestructor = true;
            }
          }
          if (hasDestructor)
            return false;
          else
            return true;
        } else if (isSgFunctionDeclaration(decl)) {
          return true;
        } else if (isSgEnumDeclaration(decl)) {
          return true;
        } else {
          return false;
        }
    }
  }

  bool isUpcSharedModifierType (SgModifierType* mod_type)
  {
    ROSE_ASSERT(mod_type);
    if (mod_type->get_typeModifier().get_upcModifier().get_isShared())
      return true;
    else 
      return false;
  }

  bool isUpcSharedArrayType (SgArrayType* array_type)
  {
    bool result = false;
    ROSE_ASSERT(array_type);
    SgType* base_type=getArrayElementType(array_type); 
    if (isSgModifierType(base_type))
      result = isUpcSharedModifierType(isSgModifierType(base_type));
    return result;
  }

  bool isUpcStrictSharedModifierType(SgModifierType* mod_type)
  {

    ROSE_ASSERT(isUpcSharedModifierType(mod_type));
    bool result = false;
    result = mod_type->get_typeModifier().get_upcModifier().isUPC_Strict();
    return result;
  }

  //! Get the block size of a UPC shared type, including Modifier types and array of modifier types (shared arrays)
  size_t getUpcSharedBlockSize(SgType* t)
  {
    SgModifierType * mod_type=NULL;  
    bool isUpc = isUpcSharedType(t,&mod_type);
    ROSE_ASSERT(isUpc);
    return getUpcSharedBlockSize(mod_type);
  }

  size_t getUpcSharedBlockSize(SgModifierType* mod_type)
  {
    ROSE_ASSERT(isUpcSharedModifierType(mod_type));

    int result =  mod_type->get_typeModifier().get_upcModifier().get_layout();
    if (result == -1 ) // unspecified block size is treated as size 1
      result = 1;
    return result;
  }

  //! Check if a type is a UPC shared type, including shared array, shared pointers etc.
  bool isUpcSharedType(SgType* t, SgModifierType ** mod_type_out/* = NULL*/)
  {
    return (hasUpcSharedType(t,mod_type_out) && !isUpcPrivateToSharedType(t));
  }

  //! Has a UPC shared type?
  bool hasUpcSharedType(SgType* sg_type,SgModifierType ** mod_type_out/*=NULL*/)
  {
    ROSE_ASSERT(sg_type);
    bool result = false;

    if (isSgModifierType(sg_type))
    {
      SgModifierType * mod_type = isSgModifierType(sg_type);
      if (isUpcSharedModifierType(mod_type))
      {  
        if (mod_type_out) *mod_type_out = mod_type;
        result = true;
      }  
      else 
        result = hasUpcSharedType( (isSgModifierType(sg_type))->get_base_type(), mod_type_out);
    }  
    else if (isSgPointerType(sg_type))
      result = hasUpcSharedType( (isSgPointerType(sg_type))->get_base_type(),mod_type_out );
    else if (isSgReferenceType(sg_type))
      result = hasUpcSharedType( (isSgReferenceType(sg_type))->get_base_type(),mod_type_out );
    else if (isSgArrayType(sg_type))
      result = hasUpcSharedType( (isSgArrayType(sg_type))->get_base_type(),mod_type_out );
    else if (isSgTypedefType(sg_type))
      result = hasUpcSharedType( (isSgTypedefType(sg_type))->get_base_type(), mod_type_out);

    if ((result == false) &&(mod_type_out))  *mod_type_out = NULL;
    return result;
  } //hasUpcSharedType

  //! Is UPC private-to-shared type? Judge the order of SgPointer and SgUPC_AccessModifier
  bool isUpcPrivateToSharedType(SgType* t)
  {
    bool check = hasUpcSharedType(t);
    if (check==false) return false;
    bool pointerFirst = false;
    SgType* currentType = t;

    while (true) 
    {
      if (isSgModifierType(currentType))
      {
        SgModifierType * mod_type = isSgModifierType(currentType);
        if (mod_type->get_typeModifier().get_upcModifier().get_isShared())
          break; //Reaches SgUPC_AccessModifier,exit the while loop
        else 
          currentType = isSgModifierType(currentType)->get_base_type();
      } // if SgModifierType
      else if (isSgPointerType(currentType))
      {
        pointerFirst = true; // reach pointer before reaching UPC modifier!
        currentType = isSgPointerType(currentType)->get_base_type();
      } // if Pointer
      else if (isSgReferenceType(currentType))
        currentType = isSgReferenceType(currentType)->get_base_type();
      else if (isSgArrayType(currentType))
        currentType =  isSgArrayType(currentType)->get_base_type();
      else if (isSgTypedefType(currentType))
        currentType =  isSgTypedefType(currentType)->get_base_type();
    } // while

    return pointerFirst;
  }// isUpcPrivateToSharedType()

  //! Phase-less means block size ==1, or 0, or unspecified(-1). 
  bool isUpcPhaseLessSharedType (SgType* t)
  {
    SgModifierType * mod_type_out;
    int block_size;

    bool check = hasUpcSharedType(t,&mod_type_out);
    //ROSE_ASSERT(hasUpcSharedType(t,mod)); // avoid side effect for assertion!!
    if (check==false) 
      return false;
    else
      ROSE_ASSERT(check&&mod_type_out); 
    block_size =  mod_type_out->get_typeModifier().get_upcModifier().get_layout();
    // cout<<"block size is "<<block_size<<endl;
    if ((block_size==1)||(block_size == -1)|| (block_size == 0))
      return true;
    else 
      return false;
  } // isUpcPhaseLessSharedType

  //! Is a UPC array with dimension of X*THREADS
  /*!
   * EDG-SAGE connection ensures that UPC array using THREADS dimension 
   *  has an index expression of type SgMultiplyOp
   * and operands (X and SgUpcThreads)
   * TODO multi dimensional arrays
   */
  bool isUpcArrayWithThreads(SgArrayType* t)
  {
    ROSE_ASSERT(t!=NULL);
    bool result = false;
    SgExpression * exp = t->get_index();
    SgMultiplyOp* multiply = isSgMultiplyOp(exp);
    if (multiply)
      if (isSgUpcThreads(multiply->get_rhs_operand()))
        result = true;
    return result;  
  }

  SgType* getFirstVarType(SgVariableDeclaration* decl)
  {
    ROSE_ASSERT(decl);
    SgVariableSymbol * sym = getFirstVarSym(decl);
    ROSE_ASSERT(sym);
    return sym->get_type();
  }

  //! Generate a type string for a given type 
  //TODO put it into sageInterface when ready 
  string mangleType(SgType* type)
  {
    string result;
    result = type_string_map[type];
    if (result.empty())
    {
      if (isScalarType(type))
        result = mangleScalarType(type);
      else if (isSgModifierType(type))  
        result = mangleModifierType(isSgModifierType(type));
      else if (isSgPointerType(type))  
        result = 'P' + mangleType(isSgPointerType(type)->get_base_type());
      else if (isSgReferenceType(type))  
        result = 'R' + mangleType(isSgPointerType(type)->get_base_type());
      //rvalue reference is not yet supported in EDG/ROSE/GCC  
      else if (isSgTypedefType(type))
        result = mangleType(isSgTypedefType(type)->get_base_type());
      else if (isSgArrayType(type))  
      {  
        size_t element_count = getArrayElementCount(isSgArrayType(type));
        stringstream ss;
        ss<<element_count;
        string len=ss.str();
        if (isUpcArrayWithThreads(isSgArrayType(type))) len = len+"H";
        result = "A" + len + "_" + mangleType(isSgArrayType(type)->get_base_type());
      }  
      else
      {  
        //TODO function-type, class-enum-type, pointer-to-member-type, template-param, etc
        cerr<<"Unhandled type mangling for "<<type->sage_class_name()<<endl;
        result = "*Unhandled*";
      }

      type_string_map[type] =  result;
      if (string_type_map[result] == 0)
        string_type_map[result] = type; 
    }     // end if
    return result;

  } // mangeType()
  //! Mangle builtin scalar types
  /*
   * isScalarType() return true for:
   *   char, short, int, long , void, Wchar, Float, double, 
   *   long long, string, bool, complex, imaginary, 
   *   including their signed and unsigned variants, if any.
   *
   * Refer to http://www.codesourcery.com/public/cxx-abi/abi.html#mangling
   *  
   */
  string mangleScalarType(SgType* type)
  { 
    string result;
    ROSE_ASSERT(isScalarType(type)); //? Not exact match to specification

    switch (type->variantT())
    {
      /* Keep the order as the same as the specification */
      case V_SgTypeVoid :
        result += "v";
        break;
      case V_SgTypeWchar:
        result += "w";
        break;
      case V_SgTypeBool:
        result += "b";
        break;
      case V_SgTypeChar :
        result += "c";
        break;
      case V_SgTypeSignedChar :
        result += "a";
        break;
      case V_SgTypeUnsignedChar :
        result += "h";
        break;
      case V_SgTypeShort :
      case V_SgTypeSignedShort: // ? 
        result += "s";
        break;
      case V_SgTypeUnsignedShort:
        result += "t";
        break;
      case V_SgTypeInt:
      case V_SgTypeSignedInt : //?
        result += "i";
        break;
      case V_SgTypeUnsignedInt :
        result += "j";
        break;
      case V_SgTypeLong :
      case V_SgTypeSignedLong :
        result += "l";
        break;
      case V_SgTypeUnsignedLong :
        result += "m";
        break;
      case V_SgTypeLongLong:
        result += "x";
        break;
      case V_SgTypeUnsignedLongLong:
        result += "y";
        break;
        // unsigned int128 // not exist in ROSE
      case V_SgTypeFloat:
        result += "f";
        break;
      case V_SgTypeDouble:
        result += "d";
        break;
      case V_SgTypeLongDouble:
        result += "e";
        break;
        /* Missing in ROSE
           n  # __int128
           o  # unsigned __int128
           g  # float 128, 
           z  # ellipsis
           Dd # IEEE 754r decimal floating point (64 bits)
           De # IEEE 754r decimal floating point (128 bits)
           Df # IEEE 754r decimal floating point (32 bits)
           Dh # IEEE 754r half-precision floating point (16 bits)
           u <source-name>      # vendor extended type
           */
      case V_SgTypeComplex:
        result += "C";
        break;
      case V_SgTypeImaginary:
        result += "G";
        break;
      case V_SgTypeString: //? TODO Not for char*: array of char ? or pointer to char ?
        // only for SgStringVal
        result = ".unhandledSgTypeString." ;
        break; 
      default:
        result = "Invalid.mtype"; //?
    } // switch

    return result;
  }
  //! <CV-qualifiers> ::= [r] [V] [K]   # restrict (C99), volatile, const
  // the order of handling const, volatile, and restric matters
  // Quote of the specification:
  // "In cases where multiple order-insensitive qualifiers are present, 
  // they should be ordered 'K' (closest to the base type), 'V', 'r', and 'U' (farthest from the base type)"
  // also UPC shared type
  string mangleModifierType(SgModifierType* type)
  {
    string result;
    if (isConstType(type))
      result = "K" ; 
    if (isVolatileType(type))
      result = "V" + result;
    if (isRestrictType(type))
      result = "r" + result;
    if (isUpcSharedModifierType(type))
    {
      size_t blocksize = getUpcSharedBlockSize(type);
      stringstream ss;
      ss << blocksize;
      result = ss.str() + "_" + result;
      if (isUpcStrictSharedModifierType(type))
        result = "S" + result;
      else
        result = "R" + result;
    }
    return result + mangleType(type->get_base_type());  
  }

} //end of namespace 

